#include "types.h"
#include "math.h"

template <DataType T1, DataType T2>
struct BinaryOperator {
  bool simd;
  int step;
  BinaryOperator() : simd(false), step(0) {};
  virtual void simdcompute(typename TypeTraits<T1>::T *a,
                           typename TypeTraits<T2>::T *b,
                           void *buff) {};
};

template <DataType T1, DataType T2>
struct AddOperator : public BinaryOperator<T1, T2> {
  enum Type { R = TypeResolver<T1, T2, BASE_MATH>::T };
  typedef typename SimdTypePatch< typename TypeTraits<T1>::T >::T SimdType;
  typedef SimdType Vec __attribute__ ((vector_size (16)));

  AddOperator() {
    this->simd = (T1 == T2) && (T2 == (DataType) R);
    this->step = sizeof(Vec) / sizeof(SimdType);
  }

  void simdcompute(typename TypeTraits<T1>::T *a,
                   typename TypeTraits<T2>::T *b,
                   void *buff) {
    *reinterpret_cast<Vec*>(buff) = *reinterpret_cast<Vec*>(a) +
                                    *reinterpret_cast<Vec*>(b);
  }

  typename TypeTraits<R>::T compute(typename TypeTraits<T1>::T a,
                                    typename TypeTraits<T2>::T b){
    return static_cast<typename TypeTraits<R>::T>(a + b);
  }
};

template <DataType T1, DataType T2>
struct SubtractOperator : public BinaryOperator<T1, T2> {
  enum Type { R = TypeResolver<T1, T2, BASE_MATH>::T };
  typedef typename SimdTypePatch< typename TypeTraits<T1>::T >::T SimdType;
  typedef SimdType Vec __attribute__ ((vector_size (16)));

  SubtractOperator() {
    this->simd = (T1 == T2) && (T2 == (DataType) R);
    this->step = sizeof(Vec) / sizeof(SimdType);
  }

  void simdcompute(typename TypeTraits<T1>::T *a,
                   typename TypeTraits<T2>::T *b,
                   void *buff) {
    *reinterpret_cast<Vec*>(buff) = *reinterpret_cast<Vec*>(a) -
                                    *reinterpret_cast<Vec*>(b);
  }

  typename TypeTraits<R>::T compute(typename TypeTraits<T1>::T a,
                                    typename TypeTraits<T2>::T b){
    return static_cast<typename TypeTraits<R>::T>(a - b);
  }
};

template <DataType T1, DataType T2>
struct MultiplyOperator : public BinaryOperator<T1, T2> {
  enum Type { R = TypeResolver<T1, T2, BASE_MATH>::T };
  typedef typename SimdTypePatch< typename TypeTraits<T1>::T >::T SimdType;
  typedef SimdType Vec __attribute__ ((vector_size (16)));

  MultiplyOperator() {
    this->simd = (T1 == T2) && (T2 == (DataType) R);
    this->step = sizeof(Vec) / sizeof(SimdType);
  }

  void simdcompute(typename TypeTraits<T1>::T *a,
                   typename TypeTraits<T2>::T *b,
                   void *buff) {
    *reinterpret_cast<Vec*>(buff) = *reinterpret_cast<Vec*>(a) *
                                    *reinterpret_cast<Vec*>(b);
  }

  typename TypeTraits<R>::T compute(typename TypeTraits<T1>::T &a,
                                    typename TypeTraits<T2>::T &b){
    return static_cast<typename TypeTraits<R>::T>(a * b);
  }
};

template <DataType T1, DataType T2>
struct FDivideOperator : public BinaryOperator<T1, T2> {
  enum Type { R = DOUBLE };

  typename TypeTraits<R>::T compute(typename TypeTraits<T1>::T &a,
                                    typename TypeTraits<T2>::T &b){
    return static_cast<typename TypeTraits<R>::T>(a) / b;
  }
};

/* logical */
template <DataType T1, DataType T2>
struct AndOperator : public BinaryOperator<T1, T2> {
  enum Type { R = BOOL };

  typename TypeTraits<R>::T compute(typename TypeTraits<T1>::T &a,
                                    typename TypeTraits<T2>::T &b){
    return a && b;
  }
};

template <DataType T1, DataType T2>
struct OrOperator : public BinaryOperator<T1, T2> {
  enum Type { R = BOOL };

  typename TypeTraits<R>::T compute(typename TypeTraits<T1>::T &a,
                                    typename TypeTraits<T2>::T &b){
    return a || b;
  }
};

/* comparison */
template <DataType T1, DataType T2>
struct EqualOperator : public BinaryOperator<T1, T2> {
  enum Type { R = BOOL };

  typename TypeTraits<R>::T compute(typename TypeTraits<T1>::T a,
                                    typename TypeTraits<T2>::T b){
    return a == b;
  }
};

template <DataType T1, DataType T2>
struct NotEqualOperator : public BinaryOperator<T1, T2> {
  enum Type { R = BOOL };

  typename TypeTraits<R>::T compute(typename TypeTraits<T1>::T a,
                                    typename TypeTraits<T2>::T b){
    return a != b;
  }
};

template <DataType T1, DataType T2>
struct LowerOperator : public BinaryOperator<T1, T2> {
  enum Type { R = BOOL };

  static const bool simd = false;

  typename TypeTraits<R>::T compute(typename TypeTraits<T1>::T a,
                                    typename TypeTraits<T2>::T b){
    return a < b;
  }
};

template <DataType T1, DataType T2>
struct GreaterOperator : public BinaryOperator<T1, T2> {
  enum Type { R = BOOL };

  static const bool simd = false;

  typename TypeTraits<R>::T compute(typename TypeTraits<T1>::T a,
                                    typename TypeTraits<T2>::T b){
    return a > b;
  }
};


/* Unary operators */
template <DataType T>
struct LogOperator {
  enum Type { R = DOUBLE };

  typename TypeTraits<R>::T compute(typename TypeTraits<T>::T a){
    return log(static_cast<typename TypeTraits<R>::T>(a));
  }
};

template <DataType T>
struct NegateOperator {
  enum Type { R = T };

  typename TypeTraits<R>::T compute(typename TypeTraits<T>::T a){
    return -a;
  }
};

template <DataType T>
struct NotOperator {
  enum Type { R = BOOL };

  typename TypeTraits<R>::T compute(typename TypeTraits<T>::T a){
    return !a;
  }
};

/* aggregation */

template <DataType T1>
struct CountOperator {
  enum Type { R = INT };

  static const bool simd = false;

  typename TypeTraits<R>::T operator() (typename TypeTraits<R>::T &old,
                                        typename TypeTraits<T1>::T &val){
    return old + 1;
  }

  static typename TypeTraits<R>::T zero(){ return 0; }
};

template <DataType T1>
struct SumOperator {
  enum Type { R = TypeResolver<T1, SUM>::T };

  static const bool simd = false;

  typename TypeTraits<R>::T operator() (typename TypeTraits<R>::T &old,
                                        typename TypeTraits<T1>::T &val){
    return old + val;
  }

  static typename TypeTraits<R>::T zero(){ return 0; }
};
