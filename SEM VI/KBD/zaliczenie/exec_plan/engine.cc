#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <fcntl.h>
#include "types.h"
#include "operations.cc"
#include "server.h"
#include "proto/operations.pb.h"

#define INIT_HASH_SIZE 10000

#define SHOW_INPUT false

const int BUF_SIZE = 1000;

class View;
class ExprOperator;
class PullableOp;
class ScanOp;
class ComputeOp;

PullableOp* op_algebraizer(Server *server, Operation root);

template<DataType T>
ExprOperator* unary_expr_algebraizer(const Expression &expr, ExprOperator *c);

template<DataType T1, DataType T2>
ExprOperator* binary_expr_algebraizer(const Expression &expr, View *source,
                                      ExprOperator *c1, ExprOperator *c2);


/* ------------- */
/* class headers */
/* ------------- */

class View {
  private:
    void **column; // columns

  public:
    DataType *type; // types
    int size;
    int length;
    bool eof;

    View(){};
    View(const ScanOperation &root);
    View(ExprOperator *expressions[], int length);
    View(const GroupByOperation &root, View *source);

    template <DataType T>
    typename TypeTraits<T>::T* get(int ix){
      return (typename TypeTraits<T>::T*) column[ix];
    }

    View* schemaClone();

    void print();
    void dump(Server *server);

    ~View();

    friend class ComputeOp;
};


class PullableOp {
  protected:
    View *buffer;

  public:
    PullableOp(View *buffer) : buffer(buffer) {};
    virtual View& pull(int length) = 0;

    friend class ComputeOp;
    friend class FilterOp;
    friend class GroupByOp;

    virtual ~PullableOp(){ delete buffer; }
};

class ScanOp : public PullableOp {
  private:
    Server *server;

    template <DataType T>
    void getColumn(int index, int number, void* dest);

  public:
    ScanOp(Server *server, const ScanOperation &scan)
      : PullableOp(new View(scan)), server(server) {};
    virtual View& pull(int length);

    virtual ~ScanOp() {}
};

class ExprOperator {
  public:
    DataType rettype;
    virtual void* pull(int length, View &source) = 0;
    ExprOperator(DataType rettype) : rettype(rettype) {};

    virtual ExprOperator* unary_alg_dispatch(const Expression &expr,
                                             ExprOperator *c) = 0;

    virtual ExprOperator* binary_alg_dispatch(const Expression &expr,
                                              View *source,
                                              ExprOperator *c1,
                                              ExprOperator *c2) = 0;

    virtual ExprOperator* binary_alg_dispatch2(DataType T1,
                                               const Expression &expr,
                                               View *source,
                                               ExprOperator *c1,
                                               ExprOperator *c2) = 0;

   virtual ~ExprOperator(){};
};


template <DataType R>
class RettypeExprOperator : public ExprOperator{
  enum Type { RetType = R };

  public:
    RettypeExprOperator() : ExprOperator(R) {};

    virtual ExprOperator* unary_alg_dispatch(const Expression &expr,
                                             ExprOperator *c){
      return unary_expr_algebraizer<R>(expr, c);
    }

    virtual ExprOperator* binary_alg_dispatch(const Expression &expr,
                                      View *source,
                                      ExprOperator *c1,
                                      ExprOperator *c2){
      return c2->binary_alg_dispatch2(R, expr, source, c1, c2);
    }

    virtual ExprOperator* binary_alg_dispatch2(DataType T1,
                                const Expression &expr, View *source,
                                ExprOperator *c1, ExprOperator *c2){
      if(T1 == INT)
        return binary_expr_algebraizer<INT, R>(expr, source, c1, c2);
      else if(T1 == DOUBLE)
        return binary_expr_algebraizer<DOUBLE, R>(expr, source, c1, c2);
      else if(T1 == BOOL)
        return binary_expr_algebraizer<BOOL, R>(expr, source, c1, c2);
      assert(false);
    }

    virtual ~RettypeExprOperator(){};
};

/* Specialized expr operators */

template <DataType T>
class ConstantExprOperator : public RettypeExprOperator<T> {
  typedef typename TypeTraits<T>::T TType;

  private:
    TType *buffer;

  public:
    ConstantExprOperator(TType val)
      : RettypeExprOperator<T>(), buffer(new TType[BUF_SIZE])
    {
      for(int i = 0; i < BUF_SIZE; i++) buffer[i] = val;
      return ;
    }

    virtual void* pull(int length, View &source){ return buffer; }

    virtual ~ConstantExprOperator(){ delete[] buffer; }
};

template <DataType T>
class ColumnExprOperator : public RettypeExprOperator<T> {
  private:
    View *source;
    int ix;

  public:
    ColumnExprOperator(View *source, int ix)
      : RettypeExprOperator<T>(), source(source), ix(ix) {};

    virtual void* pull(int length, View &source);

    virtual ~ColumnExprOperator() {}
};

template <DataType T>
class IfExprOperator : public RettypeExprOperator<T> {
  typedef typename TypeTraits<T>::T TType;
  typedef typename TypeTraits<BOOL>::T BType;

  private:
    TType *buffer;
    ExprOperator *condC, *tC, *fC;

  public:
    IfExprOperator(ExprOperator *cond, ExprOperator *t, ExprOperator *f)
      : RettypeExprOperator<T>(),
        buffer(new TType[BUF_SIZE]), condC(cond), tC(t), fC(f) {};

    virtual void* pull(int length, View &source){
      BType *cond = static_cast<BType*>(condC->pull(length, source));
      TType *t = static_cast<TType*>(tC->pull(length, source)),
            *f = static_cast<TType*>(fC->pull(length, source));

      for(int i = 0; i < length; ++i){
        buffer[i] = cond[i] ? t[i] : f[i];
      }

      return buffer;
    }

    virtual ~IfExprOperator()
    { delete buffer; delete condC; delete tC; delete fC; }
};

/* Universal binary operator */

template <DataType T1, DataType T2,
  template<DataType X, DataType Y> class Op>
class BinaryExprOperator
  : public RettypeExprOperator< (DataType) Op<T1, T2>::R >
{
  typedef typename TypeTraits<T1>::T T1Type;
  typedef typename TypeTraits<T2>::T T2Type;
  typedef typename TypeTraits< Op<T1, T2>::R >::T RetType;

  private:
    ExprOperator *child1;
    ExprOperator *child2;
    RetType *buffer;

  public:
    BinaryExprOperator(ExprOperator *c1, ExprOperator *c2)
      : RettypeExprOperator<(DataType) Op<T1, T2>::R>(),
        child1(c1), child2(c2), buffer(new RetType[BUF_SIZE]) {};

    virtual void* pull(int length, View &source){
      T1Type *a = static_cast<T1Type *>(child1->pull(length, source));
      T2Type *b = static_cast<T2Type *>(child2->pull(length, source));

      Op<T1, T2> op;
      int i = 0, step = op.step, limit = length - step;
      if(op.simd){
        #pragma omp parallel for
        for(i = 0; i < limit; i += step)
          op.simdcompute(a + i, b + i, buffer + i);
      }
      #pragma omp parallel for
      for(i = i; i < length; i++)
        buffer[i] = op.compute(a[i], b[i]);

      return buffer;
    }

    virtual ~BinaryExprOperator(){ delete child1; delete child2; delete[] buffer; }
};

/* Universal unary operator */

template <DataType T, template<DataType X> class Op>
class UnaryExprOperator
  : public RettypeExprOperator< (DataType) Op<T>::R >
{
  typedef typename TypeTraits<T>::T TType;
  typedef typename TypeTraits< Op<T>::R >::T RetType;

  private:
    ExprOperator *child;
    RetType *buffer;

  public:
    UnaryExprOperator(ExprOperator *c)
      : RettypeExprOperator<(DataType) Op<T>::R>(),
        child(c), buffer(new RetType[BUF_SIZE]) {};

    virtual void* pull(int length, View &source){
      TType *a = static_cast<TType *>(child->pull(length, source));

      Op<T> op;
      #pragma omp parallel for
      for(int i = 0; i < length; i++){
        buffer[i] = op.compute(a[i]);
      }
      return buffer;
    }

    virtual ~UnaryExprOperator(){ delete child; delete[] buffer; }
};

/* COMPUTE OPERATION */

class ComputeOp : public PullableOp {
  private:
    PullableOp *source;
    ExprOperator **expressions;
    int size;

  public:
    ComputeOp(Server *server, const ComputeOperation &node);
    virtual View& pull(int length);

    virtual ~ComputeOp();
};


/* FILTER OPERATION */

class FilterOp : public PullableOp {
  private:
    PullableOp *source;
    ExprOperator *cond;

  public:
    FilterOp(Server *server, const FilterOperation &node);
    virtual View& pull(int length);

    template <DataType T>
    void filter(typename TypeTraits<BOOL>::T *cond, int ix,
                View &data, int offset);

    virtual ~FilterOp(){ delete source; delete cond; }
};

/* GROUP BY OPERATION */

class GroupByKey {
  protected:
    void *key;

  public:
    virtual void spawn(View &v, int row, int ix) = 0;
    virtual int hash() = 0;
    virtual bool equals(const GroupByKey &b) = 0;

    virtual ~GroupByKey() {};
};

template <DataType T>
class TypedGroupByKey : public GroupByKey {
  typedef typename TypeTraits<T>::T TType;

  protected:
    TType data;

  public:
    TypedGroupByKey(View &v, int row, int ix) {
      data = v.get<T>(ix)[row];
      key = &data;
    };

    virtual void spawn(View &v, int row, int ix){
      v.get<T>(ix)[row] = data;
      return ;
    }

    virtual int hash(){
      return (int) data;
    }

    virtual bool equals(GroupByKey const &b){
      return ((TypedGroupByKey<T> &) b).data == data;
    }

    virtual ~TypedGroupByKey() {};
};

class GroupByCompositeKey {
  public:
    std::vector<GroupByKey *> keys;
    void push_key(View &v, int row, int ix, DataType type){
      GroupByKey *k;
      if(type == INT) k = new TypedGroupByKey<INT>(v, row, ix);
      else if(type == BOOL) k = new TypedGroupByKey<BOOL>(v, row, ix);
      else if(type == DOUBLE) k = new TypedGroupByKey<DOUBLE>(v, row, ix);
      else assert(false);
      keys.push_back(k);
    }

    size_t hash(){
      size_t size = keys.size(), ret = 0;
      for(size_t i = 0; i < size; ++i)
        ret += keys[i]->hash() << i % 1000000007;
      return ret;
    }

    bool equals(const GroupByCompositeKey &b){
      size_t ret = true;
      for(size_t i = 0; i < keys.size() && ret; ++i){
        ret = keys[i]->equals(*b.keys[i]);
      }
      return ret;
    }

    void spawn(View &v, int row){
      for(size_t i = 0; i < keys.size(); ++i){
        keys[i]->spawn(v, row, i);
      }
      return ;
    }

    virtual ~GroupByCompositeKey(){
      for(size_t i = 0; i < keys.size(); ++i)
        delete keys[i];
    }
};

struct CompositeKeyHash {
  std::size_t operator() (GroupByCompositeKey * const &key) const {
    return key->hash();
  }
};

struct CompositeKeyEq {
  bool operator() (GroupByCompositeKey * const &a,
                   GroupByCompositeKey * const &b) const {
    return a->equals(*b);
  }
};

class AggregationColumn {
  public:
    virtual void aggregate(View &v, int row) = 0;
    virtual void spawn(View &v, int row, int ix) = 0;
};

template<DataType T, template<DataType X> class Op>
class TypedAggregationColumn : public AggregationColumn {
  typedef typename TypeTraits< Op<T>::R >::T RetType;
  private:
    RetType val;
    int ix;

  public:
    TypedAggregationColumn(int ix) : val(Op<T>::zero()), ix(ix) {};
    virtual void aggregate(View &v, int row);
    virtual void spawn(View &v, int row, int ix);
};

class AggregationVector {
  public:
    std::vector<AggregationColumn *> fields;

    void push_col(int ix, int type, int op);
    void aggregate(View &v, int row);
    void spawn(View &v, int row, int offset);

    virtual ~AggregationVector();
};

class GroupByOp : public PullableOp {
  typedef std::unordered_map<GroupByCompositeKey *, AggregationVector *,
                             CompositeKeyHash, CompositeKeyEq> u_map;

  private:
    PullableOp *source;
    GroupByOperation operation;
    u_map map;
    bool done;
    u_map::iterator it;

  public:
    GroupByOp(Server *server, const GroupByOperation &node);
    virtual View& pull(int length);
    GroupByCompositeKey* produce_key(View &v, int row);
    AggregationVector* produce_vector(View &v, int row);

    virtual ~GroupByOp() { delete source; }
};


/* ----------------------- */
/* class View declarations */
/* ----------------------- */

View::View(const ScanOperation &scan) : eof(false)
/* View constructor for SCAN operation. */
{
  size = scan.column_size();
  type = new DataType[size];
  column = new void*[size];
  for(int i = 0; i < size; ++i){
    int t = scan.type(i);
    type[i] = static_cast<DataType>(t);
    BUFF_CREATOR(t, column[i], BUF_SIZE);
  }
  return ;
}

View::View(ExprOperator *expressions[], int size) : eof(false)
/* View constructor for COMPUTE operation. */
{
  this->size = size;
  type = new DataType[size];
  column = new void*[size];
  for(int i = 0; i < size; ++i){
    DataType t = expressions[i]->rettype;
    type[i] = t;
    BUFF_CREATOR(t, column[i], BUF_SIZE);
  }
  return ;
}

View::View(const GroupByOperation &root, View *source) : eof(false)
/* View constructor for GROUP BY operation. */
{
  size = root.group_by_column_size() + root.aggregations_size();
  type = new DataType[size];
  column = new void*[size];

  int col_size = root.group_by_column_size(),
      agg_size = root.aggregations_size();

  for(int i = 0; i < col_size; ++i){
    DataType t = source->type[root.group_by_column(i)];
    type[i] = t;
    BUFF_CREATOR(t, column[i], BUF_SIZE);
  }
  for(int i = 0; i < agg_size; ++i){
    DataType t = source->type[root.aggregations(i).aggregated_column()];
    int op = root.aggregations(i).type();

    GROUP_BY_TYPE_RESOLVE(t, op, type[i + col_size]);
    BUFF_CREATOR(type[i + col_size], column[i + col_size], BUF_SIZE);
  }
  return ;
}

View* View::schemaClone()
/* Returns an empty view with an identical schema. */{
  View *v = new View();
  v->size = size;
  v->length = 0;
  v->type = new DataType[size];
  v->column = new void*[size];
  for(int i = 0; i < size; i++){
    int t = type[i];
    v->type[i] = static_cast<DataType>(t);
    BUFF_CREATOR(t, v->column[i], BUF_SIZE);
  }
  return v;
}

void View::print()
/* Prints out a view into a standard output. */
{
  for(int j = 0; j < length; ++j){
    for(int i = 0; i < size; ++i){
      if(type[i] == INT) std::cerr << get<INT>(i)[j];
      else if(type[i] == DOUBLE) std::cerr << get<DOUBLE>(i)[j];
      else if(type[i] == BOOL) std::cerr << get<BOOL>(i)[j];
      else assert(false);
      if(i != size - 1) std::cerr << "\t";
    }
    std::cerr << std::endl;
  }
  return ;
}

void View::dump(Server *server){
  for(int i = 0; i < size; ++i){
    if(type[i] == INT) server->ConsumeInts(i, length, get<INT>(i));
    else if(type[i] == DOUBLE) server->ConsumeDoubles(i, length, get<DOUBLE>(i));
    else if(type[i] == BOOL) server->ConsumeByteBools(i, length, get<BOOL>(i));
    else assert(false);
  }
  return ;
}

View::~View(){
  delete[] column;
  delete[] type;
}

/* ----------------------- */
/* class Scan declarations */
/* ----------------------- */

View& ScanOp::pull(int length)
/* Pull operation for SCAN. */
{
  int delta = 0;
  for(int i = 0; i < buffer->size; ++i){
    int t = buffer->type[i];
    if(t == INT)
      delta = server->GetInts(i, length, buffer->get<INT>(i));
    else if(t == DOUBLE)
      delta = server->GetDoubles(i, length, buffer->get<DOUBLE>(i));
    else if(t == BOOL)
      delta = server->GetByteBools(i, length, buffer->get<BOOL>(i));
    else assert(false);
  }
  buffer->eof = (delta == 0);
  buffer->length = delta;
  if(SHOW_INPUT)  buffer->print();
  return *buffer;
}


ExprOperator* expr_algebraizer(const Expression &expr, View *source){
  int op = expr.operator_();
  /* COLUMN */
  if(Expression_Operator_COLUMN == op){
    int column = expr.column_id(), t = source->type[column];
    if(t == INT) return new ColumnExprOperator<INT>(source, column);
    else if(t == DOUBLE) return new ColumnExprOperator<DOUBLE>(source, column);
    else if(t == BOOL) return new ColumnExprOperator<BOOL>(source, column);
  }
  /* CONSTANT */
  else if(Expression_Operator_CONSTANT == op){
    if(expr.has_constant_int32()){
      return new ConstantExprOperator<INT>(expr.constant_int32());
    }
    else if(expr.has_constant_double())
      return new ConstantExprOperator<DOUBLE>(expr.constant_double());
    else if(expr.has_constant_bool()){
      return new ConstantExprOperator<BOOL>(expr.constant_bool());
    }
  }
  /* IF */
  else if(Expression_Operator_IF == op){
    ExprOperator *cond = expr_algebraizer(expr.children(0), source),
                 *t = expr_algebraizer(expr.children(1), source),
                 *f = expr_algebraizer(expr.children(2), source);
    if(t->rettype == INT)
      return new IfExprOperator<INT>(cond, t, f);
    else if(t->rettype == DOUBLE)
      return new IfExprOperator<DOUBLE>(cond, t, f);
    else if(t->rettype == BOOL)
      return new IfExprOperator<BOOL>(cond, t, f);
  }
  /* BINARY OPERATORS */
  else if(Expression_Operator_ADD == op ||
          Expression_Operator_SUBTRACT == op ||
          Expression_Operator_MULTIPLY == op ||
          Expression_Operator_FLOATING_DIVIDE == op ||
          Expression_Operator_LOWER == op ||
          Expression_Operator_GREATER == op ||
          Expression_Operator_EQUAL == op ||
          Expression_Operator_NOT_EQUAL == op ||
          Expression_Operator_NOT == op ||
          Expression_Operator_OR == op ||
          Expression_Operator_AND == op){
    ExprOperator *c1 = expr_algebraizer(expr.children(0), source),
                 *c2 = expr_algebraizer(expr.children(1), source);
    return c1->binary_alg_dispatch(expr, source, c1, c2);
  }
  /* UNARY OPERATORS */
  else if(Expression_Operator_LOG == op ||
          Expression_Operator_NEGATE == op){
    ExprOperator *c = expr_algebraizer(expr.children(0), source);
    return c->unary_alg_dispatch(expr, c);
  }
  assert(false);
}

template <DataType T1, DataType T2>
ExprOperator* binary_expr_algebraizer(const Expression &expr, View *source,
                                      ExprOperator *c1, ExprOperator *c2){
  int op = expr.operator_();

  /* aritmethical */
  if(Expression_Operator_ADD == op)
    return new BinaryExprOperator<T1, T2, AddOperator>(c1, c2);
  else if(Expression_Operator_SUBTRACT == op)
    return new BinaryExprOperator<T1, T2, SubtractOperator>(c1, c2);
  else if(Expression_Operator_MULTIPLY == op)
    return new BinaryExprOperator<T1, T2, MultiplyOperator>(c1, c2);
  else if(Expression_Operator_FLOATING_DIVIDE == op)
    return new BinaryExprOperator<T1, T2, FDivideOperator>(c1, c2);
  /* logical */
  else if(Expression_Operator_AND == op){
    assert(T1 == BOOL && T2 == BOOL);
    return new BinaryExprOperator<T1, T2, AndOperator>(c1, c2);
  }
  else if(Expression_Operator_OR == op){
    assert(T1 == BOOL && T2 == BOOL);
    return new BinaryExprOperator<T1, T2, OrOperator>(c1, c2);
  }
  /* comparison */
  else if(Expression_Operator_EQUAL == op)
    return new BinaryExprOperator<T1, T2, EqualOperator>(c1, c2);
  else if(Expression_Operator_NOT_EQUAL == op)
    return new BinaryExprOperator<T1, T2, NotEqualOperator>(c1, c2);
  else if(Expression_Operator_LOWER == op)
    return new BinaryExprOperator<T1, T2, LowerOperator>(c1, c2);
  else if(Expression_Operator_GREATER == op)
    return new BinaryExprOperator<T1, T2, GreaterOperator>(c1, c2);
  else assert(false);
}

template <DataType T>
ExprOperator* unary_expr_algebraizer(const Expression &expr, ExprOperator *c){
  int op = expr.operator_();

  if(Expression_Operator_LOG == op)
    return new UnaryExprOperator<T, LogOperator>(c);
  else if(Expression_Operator_NEGATE == op)
    return new UnaryExprOperator<T, NegateOperator>(c);
  else if(Expression_Operator_NOT == op)
    return new UnaryExprOperator<T, NotOperator>(c);
  assert(false);
}

/* ------------------------------------- */
/* class ColumnExprOperator declarations */
/* ------------------------------------- */

template<DataType T>
void* ColumnExprOperator<T>::pull(int length, View &source){
  assert(source.type[ix] == T);
  return source.get<T>(ix);
}


/* --------------------------- */
/* class CoputeOp declarations */
/* --------------------------- */

ComputeOp::ComputeOp(Server *server, const ComputeOperation &node)
  : PullableOp(NULL)
/* COMPUTE constructor. */
{
  source = op_algebraizer(server, node.source());
  expressions = new ExprOperator*[node.expressions_size()];
  size = node.expressions_size();
  for(int i = 0; i < size; ++i)
    expressions[i] = expr_algebraizer(node.expressions(i), source->buffer);
  buffer = new View(expressions, node.expressions_size());
  return ;
}

View& ComputeOp::pull(int length)
/* Pull operation for COMPUTE. */
{
  View &s = source->pull(length);
  for(int i = 0; i < buffer->size; ++i)
    buffer->column[i] = expressions[i]->pull(s.length, s);
  buffer->eof = s.eof;
  buffer->length = s.length;
  return *buffer;
}

ComputeOp::~ComputeOp(){
  for(int i = 0; i < size; ++i)
    delete expressions[i];
  delete[] expressions;
  delete source;
}

/* --------------------------- */
/* class FilterOp declarations */
/* --------------------------- */

FilterOp::FilterOp(Server *server, const FilterOperation &node)
  : PullableOp(NULL)
{
  source = op_algebraizer(server, node.source());
  cond = expr_algebraizer(node.expression(), source->buffer);
  buffer = source->buffer->schemaClone();
  return ;
}

View& FilterOp::pull(int length)
/* Pull operation FILTER. */
{
  typename TypeTraits<BOOL>::T *c;
  int l = 0;
  bool eof = false;

  buffer->length = 0;
  do {
    View &s = source->pull(length - l);

    c = static_cast<TypeTraits<BOOL>::T *>(cond->pull(length - l, s));
    for(int i = 0; i < buffer->size; ++i){
      if(buffer->type[i] == INT) filter<INT>(c, i, s, buffer->length);
      else if(buffer->type[i] == DOUBLE) filter<DOUBLE>(c, i, s, buffer->length);
      else if(buffer->type[i] == BOOL) filter<BOOL>(c, i, s, buffer->length);
    }
    for(int i = 0; i < s.length; ++i){
      l += !!c[i];
    }
    eof = s.eof;
    buffer->length = l;
  } while(!eof && l < 3 * length / 4);

  buffer->eof = eof;

  return *buffer;
}

template <DataType T>
void FilterOp::filter(TypeTraits<BOOL>::T *cond, int ix, View &data, int offset)
/* Column filter operation. */
{
  typename TypeTraits<T>::T *source_col = data.get<T>(ix),
                            *dest_col = buffer->get<T>(ix);
  for(int i = 0, j = offset; i < data.length; ++i){
    dest_col[j] = source_col[i];
    j += cond[i];
  }
  return ;
}

/* --------------- */
/* class GroupByOp */
/* --------------- */

GroupByOp::GroupByOp(Server *server, const GroupByOperation &node)
  : PullableOp(NULL)
/* GROUP BY constructor. */
{
  source = op_algebraizer(server, node.source());
  buffer = new View(node, source->buffer);
  done = false;
  operation = node;
  map.rehash(INIT_HASH_SIZE);
  return ;
}

View& GroupByOp::pull(int length)
/* Pull operation for GROUP BY. */
{
  View *v;
  int i;

  if(!done){
    do {
      v = &(source->pull(length));
      for(int i = 0; i < v->length; ++i){
        GroupByCompositeKey *key = produce_key(*v, i);
        AggregationVector *vec;
        it = map.find(key);
        if(it == map.end()){ // insert
          vec = produce_vector(*v, i);
          map.insert(std::make_pair(key, vec));
        } else {
          vec = it->second;
          delete key;
        }
        vec->aggregate(*v, i);
      }
    } while(!v->eof);
    done = true;
    it = map.begin();
  }

  for(i = 0; i < length && it != map.end(); ++i, it++){
    it->first->spawn(*buffer, i);
    it->second->spawn(*buffer, i, it->first->keys.size());
    delete it->first; // memory utilization
    delete it->second;
  }
  map.erase(map.begin(), it);
  it = map.begin();
  buffer->length = i;
  buffer->eof = (it == map.end());

  return *buffer;
}

GroupByCompositeKey* GroupByOp::produce_key(View &v, int row)
/* Produces a GROUP BY key based on an Operation object and a data. */
{
  GroupByCompositeKey *key = new GroupByCompositeKey();
  for(int i = 0; i < operation.group_by_column_size(); ++i){
    int ix = operation.group_by_column(i);
    DataType t = v.type[ix];
    key->push_key(v, row, ix, t);
  }
  return key;
}

AggregationVector* GroupByOp::produce_vector(View &v, int row)
/* Produces a GROUP BY aggregation vector based on Operation object *
 * and a data.                                                      */
{
  AggregationVector *vec = new AggregationVector();
  for(int i = 0; i < operation.aggregations_size(); ++i){
    int ix = operation.aggregations(i).aggregated_column(),
        op = operation.aggregations(i).type();
    if(op == Aggregation_Type_COUNT) ix = 0;
    DataType t = v.type[ix];
    vec->push_col(ix, t, op);
  }
  return vec;
}


/* ---------------------------- */
/* class TypedAggregationColumn */
/* ---------------------------- */

template<DataType T, template<DataType X> class Op>
void TypedAggregationColumn<T, Op>::aggregate(View &v, int row)
/* Aggregation of single field. */
{
  Op<T> op;
  val = op(val, v.get<T>(ix)[row]);
  return ;
}

template<DataType T, template<DataType X> class Op>
void TypedAggregationColumn<T, Op>::spawn(View &v, int row, int ix)
/* Writes aggregated data into given address. */
{
  v.get< (DataType)Op<T>::R >(ix)[row] = val;
  return ;
}


/* ----------------------- */
/* class AggregationVector */
/* ----------------------- */

void AggregationVector::push_col(int ix, int type, int op)
/* Adds an aggregation field into a vector. */
{
  AggregationColumn *col;
  AGGVEC_PUSH_CREATOR(type, op, ix, col);
  fields.push_back(col);
  return ;
}

void AggregationVector::aggregate(View &v, int row)
/* Aggregates a given row from a given data. */
{
  int size = fields.size();
  for(int i = 0; i < size; ++i)
    fields[i]->aggregate(v, row);
  return ;
}

void AggregationVector::spawn(View &v, int row, int offset)
/* Writes a vector content into a given row of a given */
{
  int size = fields.size();
  for(int i = 0; i < size; ++i)
    fields[i]->spawn(v, row, i + offset);
  return ;
}

AggregationVector::~AggregationVector(){
  for(size_t i = 0; i < fields.size(); ++i)
    delete fields[i];
}

/* op_algebraizer ... MOVE? */

PullableOp* op_algebraizer(Server *server, Operation root){
  if(root.has_scan())
    return new ScanOp(server, root.scan());
  else if(root.has_compute())
    return new ComputeOp(server, root.compute());
  else if(root.has_filter())
    return new FilterOp(server, root.filter());
  else if(root.has_group_by())
    return new GroupByOp(server, root.group_by());
  else assert(false);
}

/* MAIN */

int main(int argc, char *argv[]){
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  if(argc < 3){
    std::cout << "not enough parameters" << std::endl;
    exit(1);
  }

  int fd = open(argv[2], O_RDONLY);
  Operation root;
  google::protobuf::io::FileInputStream input(fd);
  google::protobuf::TextFormat::Parse(&input, &root);
  Server *server = CreateServer(atoi(argv[1]));

  PullableOp *query = op_algebraizer(server, root);
  View *data = NULL;

  do {
    data = &query->pull(BUF_SIZE);
    data->dump(server);
  } while(!data->eof);

  delete query;

  return 0;
}
