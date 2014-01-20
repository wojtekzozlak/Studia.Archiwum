#include <sstream>
#include <vector>
#include "Assembler.h"
#include "Eval.h"
#include "core/Types.h"
#include "Operations.h"

#define align(n, size) ((n) & (-(size))) // aligns up
#define call_align(n) (-(n)) % 16 == 8 ? (n) : (n) - 8;

namespace x86 {

namespace __op = Operations;
using boost::dynamic_pointer_cast;


std::string make_meth_name(Core::FunType::PtrType fun) {
  std::stringstream ss;
  assert(fun->cls.get() != NULL);
  ss << ".cls." << fun->cls->name << "." << fun->name;
  return ss.str();
};


std::string make_bp_offset(int offset) {
  std::ostringstream ss;
  ss << offset << "(%ebp)";
  return ss.str();
};


std::string make_sp_offset(int offset) {
  std::ostringstream ss;
  ss << offset << "(%esp)";
  return ss.str();
};


std::string make_label(std::string fun, const char* lab, int ix) {
  std::ostringstream ss;
  ss << fun << "." << lab << "." << ix;
  return ss.str();
};


// {{{ Assembler

Assembler::Assembler(Core::Environment* env) : env(env) {};


void Assembler::assembly() {
  ConstStringMapping strings;

  std::cout << ".text" << std::endl << std::endl;

  Core::Environment::FunMap::iterator fun_it;
  for (fun_it = env->fun.begin(); fun_it != env->fun.end(); fun_it++) {
    assemblyFunction(fun_it->second, &strings);
  }

  Core::Environment::ClsMap::iterator cls_it;
  for (cls_it = env->cls.begin(); cls_it != env->cls.end(); cls_it++) {
    Core::ClassType::PtrType cls = cls_it->second;

    Core::ClassType::AttrMap::iterator attr_it;
    for (attr_it = cls->attrs.begin(); attr_it != cls->attrs.end(); attr_it++) {
      if (attr_it->second->getType() == Core::FUNCTION)
        assemblyMethod(dynamic_pointer_cast<Core::FunType>(attr_it->second),
            cls, &strings);
    }

    assemblyVTable(cls);
  }

  std::cout << ".data" << std::endl << std::endl;
  strings.produce(std::cout);
};


void Assembler::assemblyVTable(Core::ClassType::PtrType cls) {
  const std::vector<std::string>& vtable = cls->produceVTable();

  std::cout << ".vtable." << cls->name << ":" << std::endl;
  for (int i = 0; i < vtable.size(); i++) {
    Core::FunType::PtrType fun = dynamic_pointer_cast<Core::FunType>(
        cls->getAttr(vtable[i]));
    std::cout << "	.4byte	" << make_meth_name(fun) << std::endl;
  }
  std::cout << std::endl << std::endl;
};


void Assembler::assemblyFunction(Core::FunType::PtrType fun,
    ConstStringMapping* strings) {
  if (fun->body == NULL) return;

  AssemblyEnv ass_env(env, fun->name, std::cout, strings);
  Core::AbstractScope::PtrType global_scope(new Core::GlobalScope(env));

  Core::BlockScope::PtrType function_scope(new Core::BlockScope(global_scope));
  for (int i = 0; i < fun->args.size(); i++) {
    function_scope->define(fun->idents[i], fun->args[i]->toLValue());
    int arg_ix = function_scope->get(fun->idents[i]).first;
    ass_env.offset_mapping.insert(std::make_pair(arg_ix, 8 + 4 * i));
  }

  Core::BlockScope::PtrType body_scope(new Core::BlockScope(function_scope));

  std::cout << ".global " << fun->name << std::endl;
  std::cout << fun->name << ":" << std::endl;
  std::cout << "	push	%ebp" << std::endl;
  std::cout << "	movl	%esp, %ebp" << std::endl;


  StmtAssembler ass(body_scope, &ass_env, 0);
  fun->body->sblock_->accept(&ass);

  std::cout << fun->name << ".end:" << std::endl;
  std::cout << "	leave" << std::endl;
  std::cout << "	ret" << std::endl << std::endl;
};


void Assembler::assemblyMethod(Core::FunType::PtrType fun,
    Core::ClassType::PtrType cls, ConstStringMapping* strings) {
  std::string name = make_meth_name(fun);

  AssemblyEnv ass_env(env, name, std::cout, strings);
  Core::AbstractScope::PtrType global_scope(new Core::GlobalScope(env));
  Core::AbstractScope::PtrType class_scope(new Core::ClassScope(global_scope,
      cls.get()));

  Core::BlockScope::PtrType function_scope(new Core::BlockScope(global_scope));
  function_scope->define("self", boost::dynamic_pointer_cast<Core::AbstractType>
      (cls));
  ass_env.offset_mapping.insert(std::make_pair(0, 8));
  assert(function_scope->get("self").first == 0);
 
  for (int i = 0; i < fun->args.size(); i++) {
    function_scope->define(fun->idents[i], fun->args[i]->toLValue());
    int arg_ix = function_scope->get(fun->idents[i]).first;
    ass_env.offset_mapping.insert(std::make_pair(arg_ix, 8 + 4 * (i + 1)));
  }

  Core::BlockScope::PtrType body_scope(new Core::BlockScope(function_scope));

  std::cout << ".global " << name << std::endl;
  std::cout << name << ":" << std::endl;
  std::cout << "	push	%ebp" << std::endl;
  std::cout << "	movl	%esp, %ebp" << std::endl;


  StmtAssembler ass(body_scope, &ass_env, 0);
  fun->body->sblock_->accept(&ass);

  std::cout << name << ".end:" << std::endl;
  std::cout << "	leave" << std::endl;
  std::cout << "	ret" << std::endl << std::endl;
};

// }}}


// {{{ AssemblyEnv

AssemblyEnv::AssemblyEnv(Core::Environment* env, std::string fun_name,
    std::ostream& stream, ConstStringMapping* strings) : env(env),
    fun_name(fun_name), stream(stream), strings(strings) {};

// AssemblyEnv }}}


// {{{ ConstStringMapping

ConstStringMapping::ConstStringMapping() : next(0) {};


std::string ConstStringMapping::escape(const std::string& str) {
  std::stringstream ss;
  const char* c_str = str.c_str();

  for (int i = 0; i < str.size(); i++) {
    char c = c_str[i];
    if (c == '"')
      ss << '\\' << c;
    else if (c == '\n')
      ss << "\\n";
    else if (c == '\t')
      ss << "\\t";
    else
      ss << c;
  }
  return ss.str();
};


int ConstStringMapping::get(std::string str) {
  MapType::iterator it = map.find(str);

  if (it != map.end()) {
    return it->second;
  } else {
    map.insert(std::make_pair(str, next));
    next++;
    return next - 1;
  }
};


void ConstStringMapping::produce(std::ostream& stream) {
  MapType::iterator it;
  for (it = map.begin(); it != map.end(); it++) {
    stream << ".STR" << it->second << ":" << std::endl;
    stream << "	.string \"" << escape(it->first) << "\"" << std::endl;
  }
};

// ConstStringMapping }}}


// {{{ TypeSize

int TypeSize::getSize(Core::AbstractType* t) {
  TypeSize worker;
  t->accept(&worker);
  return worker.retval;
};


void TypeSize::visitInt(Core::IntType* t) {
  retval = 4;
};


void TypeSize::visitVoid(Core::VoidType* t) {
  assert(false);
  retval = 0;
};


void TypeSize::visitString(Core::StringType* t) {
  retval = 4;
};


void TypeSize::visitInvalid(Core::InvalidType* t) {
  assert(false);
  retval = 0;
};


void TypeSize::visitBool(Core::BoolType* t) {
  retval = 1;
};


void TypeSize::visitFun(Core::FunType* t) {
  assert(false);
  retval = 4;
};


void TypeSize::visitArray(Core::ArrayType* t) {
  retval = 4;
};


void TypeSize::visitClass(Core::ClassType* t) {
  retval = 4;
};


void TypeSize::visitNull(Core::NullType* t) {
  retval = 4;
};

// }}}


// {{{ StmtAssembler

bool isReturn(Visitable* op) {
  Ret* ret = dynamic_cast<Ret*>(op);
  VRet* vret = dynamic_cast<VRet*>(op);
  return (ret != NULL) || (vret != NULL) ;
};


StmtAssembler::StmtAssembler(Core::BlockScope::PtrType scope,
    AssemblyEnv* ass_env, int sp) : scope(scope), ass_env(ass_env), sp(sp) {};


void StmtAssembler::visitListStmt(ListStmt* p) {
  ListStmt::iterator it;
  for (it = p->begin(); it != p->end(); it++) {
    (*it)->accept(this);
    if (isReturn(*it)) {
      break;
    }
  }
};


void StmtAssembler::visitBlock(Block* p) {
  Core::BlockScope::PtrType new_scope(new Core::BlockScope(scope));
  StmtAssembler stmt_ass(new_scope, ass_env, sp);
  p->liststmt_->accept(&stmt_ass);
};


void StmtAssembler::visitBStmt(BStmt* p) {
  p->sblock_->accept(this);
};


void StmtAssembler::visitEmpty(Empty* p) {};


void StmtAssembler::visitSExp(SExp* p) {
  ExprAssembler expr_ass(scope, ass_env);

  if (p->expr_->type->getType() == Core::BOOL)
    expr_ass.eval(p->expr_, "%al", sp);
  else
    expr_ass.eval(p->expr_, "%eax", sp);
};


void StmtAssembler::visitCond(Cond* p) {
  std::ostream& out = ass_env->stream;
  int ix = scope->nextIx();
  std::string if_true = make_label(ass_env->fun_name, "Cond_true", ix);
  std::string if_false = make_label(ass_env->fun_name, "Cond_end", ix);

  ExprAssembler expr_ass(scope, ass_env);
  expr_ass.eval(p->expr_, if_true, if_false, sp);

  __op::lab(out, if_true);

  Core::BlockScope::PtrType true_scope(new Core::BlockScope(scope));
  StmtAssembler true_checker(true_scope, ass_env, sp);
  p->stmt_->accept(&true_checker);

  __op::lab(out, if_false);
};


void StmtAssembler::visitCondElse(CondElse* p) {
  std::ostream& out = ass_env->stream;
  int ix = scope->nextIx();
  std::string if_true = make_label(ass_env->fun_name, "CondElse_true", ix);
  std::string if_false = make_label(ass_env->fun_name, "CondElse_false", ix);
  std::string end = make_label(ass_env->fun_name, "CondElse_end", ix);

  ExprAssembler expr_ass(scope, ass_env);
  expr_ass.eval(p->expr_, if_true, if_false, sp);

  __op::lab(out, if_true);

  Core::BlockScope::PtrType true_scope(new Core::BlockScope(scope));
  StmtAssembler true_checker(true_scope, ass_env, sp);
  p->stmt_1->accept(&true_checker);

  __op::jmp(out, end);
  __op::lab(out, if_false);

  Core::BlockScope::PtrType false_scope(new Core::BlockScope(scope));
  StmtAssembler false_checker(false_scope, ass_env, sp);
  p->stmt_2->accept(&false_checker);

  __op::lab(out, end);
};


void StmtAssembler::visitWhile(While* p) {
  std::ostream& out = ass_env->stream;
  int ix = scope->nextIx();
  std::string body = make_label(ass_env->fun_name, "While_body", ix);
  std::string end = make_label(ass_env->fun_name, "While_end", ix);
  std::string cond = make_label(ass_env->fun_name, "While_cond", ix);

  __op::lab(out, cond);

  ExprAssembler expr_ass(scope, ass_env);
  expr_ass.eval(p->expr_, body, end, sp);

  __op::lab(out, body);

  Core::BlockScope::PtrType new_scope(new Core::BlockScope(scope));
  StmtAssembler body_ass(new_scope, ass_env, sp);
  p->stmt_->accept(&body_ass);

  __op::jmp(out, cond);
  __op::lab(out, end);
};


void StmtAssembler::visitFor(For* p) {
  int label_ix = scope->nextIx();
  std::ostream& out = ass_env->stream;

  int type_size = TypeSize::getSize(p->type_->type.get());
  int array_p = align(sp - 4, 4);
  int ix_p = array_p - 4;
  int len_p = ix_p - 4;
  int var_p = align(len_p - type_size, type_size);

  std::string array_str = make_bp_offset(array_p);
  std::string len_str = make_bp_offset(len_p);
  std::string ix_str = make_bp_offset(ix_p);
  std::string var_str = make_bp_offset(var_p);

  std::string start = make_label(ass_env->fun_name, "FOR_START", label_ix);
  std::string end = make_label(ass_env->fun_name, "FOR_END", label_ix);

  ExprAssembler ass(scope, ass_env);
  ass.eval(p->expr_, array_str, sp);

  EAttr get_len(p->expr_, std::string("length"));
  get_len.type = Core::IntType::obj();
  ass.eval(&get_len, len_str, array_p);
  get_len.expr_ = NULL;

  __op::movl(out, 0, ix_str);
  __op::lab(out, start);
  __op::cmpl(out, ix_str, len_str);
  __op::cndjmp(out, "jnl", end);

  __op::movregl(out, array_str, "%ecx");
  __op::movregl(out, ix_str, "%edx");

  if (type_size == 4) {
    __op::movregl(out, "(%ecx, %edx, 4)", "%eax");
    __op::movregl(out, "%eax", var_str);
  } else {
    __op::movregb(out, "(%ecx, %edx, 1)", "%al");
    __op::movregb(out, "%al", var_str);
  }
  
  Core::BlockScope::PtrType for_scope(new Core::BlockScope(scope));
  for_scope->define(p->ident_, p->type_->type);
  ass_env->offset_mapping.insert(std::make_pair(for_scope->get(p->ident_).first,
      var_p));
  StmtAssembler body_ass(for_scope, ass_env, var_p);
  p->stmt_->accept(&body_ass);


  __op::incl(out, ix_str);
  __op::jmp(out, start);
  __op::lab(out, end);
};


void StmtAssembler::visitAss(Ass* p) {
  std::ostream& out = ass_env->stream;

  EVar* var_node = dynamic_cast<EVar*>(p->expr_1);
  EGetArray* arr_node = dynamic_cast<EGetArray*>(p->expr_1);
  EAttr* attr_node = dynamic_cast<EAttr*>(p->expr_1);

  if (var_node != NULL) {
    int offset = ass_env->offset_mapping.find(scope->get(var_node->ident_).first)
        ->second;
    ExprAssembler expr_ass(scope, ass_env);
    expr_ass.eval(p->expr_2, make_bp_offset(offset), sp);

  } else if(arr_node != NULL || attr_node != NULL) {
    int type_size = TypeSize::getSize(p->expr_1->type.get());
    int ref_p = align(sp - 4, 4);
    int val_p = align(ref_p - 4, 4);
    std::string ref_str = make_bp_offset(ref_p);
    std::string val_str = make_bp_offset(val_p);

    ExprAssembler exp_ass(scope, ass_env);
    exp_ass.retLValue().eval(p->expr_1, ref_str, sp);
    exp_ass.eval(p->expr_2, val_str, ref_p);

    if (type_size == 4) {
      __op::movregl(out, val_str, "%edx");
      __op::movregl(out, ref_str, "%ecx");
      __op::movregl(out, "%edx", "(%ecx)");
    } else if (type_size == 1) {
      __op::movregb(out, val_str, "%dl");
      __op::movregl(out, ref_str, "%ecx");
      __op::movregb(out, "%dl", "(%ecx)");
    } else assert(false);
  } else assert(false);
};


void StmtAssembler::visitRet(Ret* p) {
  ExprAssembler expr_ass(scope, ass_env);
  if (p->expr_->type->getType() == Core::BOOL)
    expr_ass.eval(p->expr_, "%al", sp);
  else
    expr_ass.eval(p->expr_, "%eax", sp);
  __op::jmp(ass_env->stream, ass_env->fun_name + ".end");
};


void StmtAssembler::visitVRet(VRet* p) {
  __op::jmp(ass_env->stream, ass_env->fun_name + ".end");
};


void StmtAssembler::visitDecr(Decr* p) {
  std::ostream& out = ass_env->stream;
  EVar* var_node = dynamic_cast<EVar*>(p->expr_);
  EGetArray* arr_node = dynamic_cast<EGetArray*>(p->expr_);
  EAttr* attr_node = dynamic_cast<EAttr*>(p->expr_);

  if (var_node != NULL) {
    int offset = ass_env->offset_mapping.find(scope->get(var_node->ident_).first)
        ->second;
    __op::decl(out, make_bp_offset(offset));
  } else if(arr_node != NULL || attr_node != NULL) {
    ExprAssembler exp_ass(scope, ass_env);
    exp_ass.retLValue().eval(p->expr_, "%eax", sp);
    __op::decl(out, "(%eax)");
  } else assert(false);
};


void StmtAssembler::visitIncr(Incr* p) {
  std::ostream& out = ass_env->stream;
  EVar* var_node = dynamic_cast<EVar*>(p->expr_);
  EGetArray* arr_node = dynamic_cast<EGetArray*>(p->expr_);
  EAttr* attr_node = dynamic_cast<EAttr*>(p->expr_);

  if (var_node != NULL) {
    int offset = ass_env->offset_mapping.find(scope->get(var_node->ident_).first)
        ->second;
    __op::incl(out, make_bp_offset(offset));
  } else if(arr_node != NULL || attr_node != NULL) {
    ExprAssembler exp_ass(scope, ass_env);
    exp_ass.retLValue().eval(p->expr_, "%eax", sp);
    __op::incl(out, "(%eax)");
  } else assert(false);
};


void StmtAssembler::visitDecl(Decl* p) {
  declBuf = p->type_->type;

  ListItem::iterator it;
  for (it = p->listitem_->begin(); it != p->listitem_->end(); it++) {
    (*it)->accept(this);
  };
};


void StmtAssembler::visitInit(Init* p) {
  int type_size = TypeSize::getSize(declBuf.get());
  int new_sp = align(sp - type_size, type_size);
  sp = new_sp;

  ExprAssembler expr_ass(scope, ass_env);
  expr_ass.eval(p->expr_, make_bp_offset(sp), sp);

  scope->define(p->ident_, declBuf);
  ass_env->offset_mapping.insert(std::make_pair(scope->get(p->ident_).first,
      new_sp));
};


void StmtAssembler::visitNoInit(NoInit* p) {
  int type_size = TypeSize::getSize(declBuf.get());
  scope->define(p->ident_, declBuf);
  int new_sp = align(sp - type_size, type_size);
  ass_env->offset_mapping.insert(std::make_pair(scope->get(p->ident_).first,
      new_sp));
  sp = new_sp;

  ExprAssembler ass(scope, ass_env);
  ass.dest = make_bp_offset(sp);
  ass.sp = sp;
  OpEval(declBuf, p, &ass);
}

// StmtAssembler }}}


// {{{ ExprAssembler

ExprAssembler::ExprAssembler(Core::BlockScope::PtrType scope,
    AssemblyEnv* ass_env) : scope(scope), ass_env(ass_env), lvalue(false) {};


void ExprAssembler::eval(Visitable* p, std::string dest, int sp) {
  this->dest = dest;
  this->sp = sp;

  p->accept(this);
  this->lvalue = false;
};


void ExprAssembler::eval(Visitable* p, std::string if_true,
    std::string if_false, int sp) {
  this->if_true = if_true;
  this->if_false = if_false;
  this->sp = sp;

  p->accept(this);
  this->lvalue = false;
};


ExprAssembler& ExprAssembler::retLValue() {
  this->lvalue = true;
  return *this;
};


void ExprAssembler::binaryExpr(Expr* arg1, Expr* arg2, Visitable* op) {
  ExprAssembler exp_ass(scope, ass_env);

  assert(arg1->type.get() != NULL);
  int type_size1 = TypeSize::getSize(arg1->type.get());
  int sp1 = align(sp - type_size1, type_size1);
  this->arg1 = make_bp_offset(sp1);
  exp_ass.eval(arg1, this->arg1, sp);

  assert(arg2->type.get() != NULL);
  int type_size2 = TypeSize::getSize(arg2->type.get());
  int sp2 = align(sp1 - type_size2, type_size2);
  this->arg2 = make_bp_offset(sp2);
  exp_ass.eval(arg2, this->arg2, sp1);

  sp = sp2;

  OpEval(arg1->type, op, this);
};


void ExprAssembler::visitEString(EString* p) {
  int ix = ass_env->strings->get(p->string_);
  std::stringstream ss;
  ss << ".STR" << ix;
  __op::movlabl(ass_env->stream, ss.str(), this->dest);
};


void ExprAssembler::visitELitTrue(ELitTrue* p) {
  if (if_true.compare(std::string()) == 0)
    __op::movb(ass_env->stream, 1, this->dest);
  else
    __op::jmp(ass_env->stream, this->if_true);
};


void ExprAssembler::visitELitFalse(ELitFalse* p) {
  if (if_true.compare(std::string()) == 0)
    __op::movb(ass_env->stream, 0, this->dest);
  else
    __op::jmp(ass_env->stream, this->if_false);
};


void ExprAssembler::visitELitInt(ELitInt* p) {
  __op::movl(ass_env->stream, p->integer_, this->dest);
};


void ExprAssembler::visitEVar(EVar* p) {
  Core::AbstractScope::EnumeratedVar var = scope->get(p->ident_);

  // Pass function to apply node.
  if (var.second->getType() == Core::FUNCTION) {
    fun = (Core::FunType*) var.second.get();
    return ;
  }

  OffsetMapping::iterator it = ass_env->offset_mapping.find(var.first);
  assert(it != ass_env->offset_mapping.end());
  int offset = it->second;

  this->arg1 = make_bp_offset(offset);
  OpEval(scope->get(p->ident_).second, p, this);
};


void ExprAssembler::visitERel(ERel* p) {
  binaryExpr(p->expr_1, p->expr_2, p->relop_);
};


void ExprAssembler::visitEAdd(EAdd* p) {
  binaryExpr(p->expr_1, p->expr_2, p->addop_);
};


void ExprAssembler::visitEMul(EMul* p) {
  binaryExpr(p->expr_1, p->expr_2, p->mulop_);
};


void ExprAssembler::visitNeg(Neg* p) {
  ExprAssembler exp_ass(scope, ass_env);

  int type_size = TypeSize::getSize(p->type.get());
  int new_sp = align(sp - type_size, type_size);
  this->arg1 = make_bp_offset(new_sp);
  exp_ass.eval(p->expr_, this->arg1, new_sp);

  OpEval(p->type, p, this);
};


void ExprAssembler::crystalizeBool(std::string tru, std::string fal, std::string end)
    {
  std::ostream& out = ass_env->stream;

  __op::lab(out, tru);
  __op::movb(out, 1, dest);
  __op::jmp(out, end);

  __op::lab(out, fal);
  __op::movb(out, 0, dest);
  __op::lab(out, end);
};


void ExprAssembler::visitNot(Not* p) {
  ExprAssembler exp_ass(scope, ass_env);

  if (if_true.compare(std::string()) == 0) {
    int ix = scope->nextLab();
    std::string if_true = make_label(ass_env->fun_name, "Not_T", ix);
    std::string if_false = make_label(ass_env->fun_name, "Not_F", ix);
    std::string end = make_label(ass_env->fun_name, "Not_end", ix);

    exp_ass.eval(p->expr_, if_false, if_true, sp);
    crystalizeBool(if_true, if_false, end);
  } else {
    exp_ass.eval(p->expr_, this->if_false, this->if_true, sp);
  }
};


void ExprAssembler::visitEAnd(EAnd* p) {
  ExprAssembler exp_ass(scope, ass_env);

  int ix = scope->nextLab();
  bool crystalize = this->if_true.compare(std::string()) == 0;

  std::string if_true = this->if_true;
  std::string if_false = this->if_false;
  std::string snd = make_label(ass_env->fun_name, "And_snd", ix);
  std::string end = make_label(ass_env->fun_name, "And_end", ix);

  if (crystalize) {
    if_true = make_label(ass_env->fun_name, "And_T", ix);
    if_false = make_label(ass_env->fun_name, "And_F", ix);
  };

  exp_ass.eval(p->expr_1, snd, if_false, sp);
  __op::lab(ass_env->stream, snd);
  exp_ass.eval(p->expr_2, if_true, if_false, sp);

  if (crystalize) {
    crystalizeBool(if_true, if_false, end);
  }
};


void ExprAssembler::visitEOr(EOr* p) {
  ExprAssembler exp_ass(scope, ass_env);

  int ix = scope->nextLab();
  bool crystalize = this->if_true.compare(std::string()) == 0;

  std::string if_true = this->if_true;
  std::string if_false = this->if_false;
  std::string snd = make_label(ass_env->fun_name, "Or_snd", ix);
  std::string end = make_label(ass_env->fun_name, "Or_end", ix);

  if (crystalize) {
    if_true = make_label(ass_env->fun_name, "Or_T", ix);
    if_false = make_label(ass_env->fun_name, "Or_F", ix);
  };

  exp_ass.eval(p->expr_1, if_true, snd, sp);
  __op::lab(ass_env->stream, snd);
  exp_ass.eval(p->expr_2, if_true, if_false, sp);

  if (crystalize) {
    crystalizeBool(if_true, if_false, end);
  }
};


void ExprAssembler::visitEApp(EApp* p) {
  std::ostream& out = ass_env->stream;
  Core::FunType *fun = dynamic_cast<Core::FunType*>(p->expr_->type.get());
  assert(fun != NULL);
  
  EAttr* attr_node = dynamic_cast<EAttr*>(p->expr_);
  bool method_call = attr_node != NULL;

  int args_size = fun->args.size() + (method_call ? 1 : 0);
  int call_sp = align(sp - args_size * 4, 16);
  call_sp = call_align(call_sp);
  __op::movsp(out, call_sp);

  ListExpr::iterator args_it;
  for (int i = p->listexpr_->size() - 1; i >= 0; i--) {
    int j = method_call ? i + 1 : i;
    ExprAssembler ass(scope, ass_env);
    ass.eval((*p->listexpr_)[i], make_sp_offset(j * 4), call_sp + j * 4);
  }

  if (method_call) {
    Core::ClassType* cls = dynamic_cast<Core::ClassType*>(
        attr_node->expr_->type.get());
    assert(cls != NULL);

    // Eval object.
    ExprAssembler ass(scope, ass_env);
    ass.eval(attr_node->expr_, make_sp_offset(0), call_sp);

    // Eval method pointer.
    int method_offset = cls->methOffset(fun->name);
    __op::movregl(out, "(%esp)", "%edx");
    __op::movregl(out, "-4(%edx)", "%ecx");
    __op::movl(out, method_offset, "%eax");
    __op::movregl(out, "(%ecx, %eax)", "%edx");
  }

  if (method_call)
    __op::call(out, "*%edx");
  else
    __op::call(out, fun->name);

  // Maybe de-crystalize bool retvalue.
  if (this->if_true.compare(std::string()) == 0) {
    __op::movl(out, "%eax", this->dest);
  } else {
    __op::cmpb(out, "%al", "$1"); // risky hack
    __op::cndjmp(out, "jz", this->if_true);
    __op::jmp(out, this->if_false);
  }
};


void ExprAssembler::visitEGetArray(EGetArray* p) {
  binaryExpr(p->expr_1, p->expr_2, p);
};


void ExprAssembler::visitENewArray(ENewArray* p) {
  std::ostream& out = ass_env->stream;
  int type_size = TypeSize::getSize(p->type_->type.get());
  
  int calloc_sp = align(sp - 8, 16);
  calloc_sp = call_align(calloc_sp);

  __op::movsp(out, calloc_sp);
  __op::movl(out, type_size, make_sp_offset(4));

  ExprAssembler ass(scope, ass_env);
  ass.eval(p->expr_, make_sp_offset(0), calloc_sp);
  // Add space for length.
  __op::addl(out, (type_size == 1 ? 4 : 1), make_sp_offset(0));
 
  __op::call(out, "calloc");
  __op::movregl(out, make_sp_offset(0), "%edx");
  // Back to real length.
  __op::subl(out, (type_size == 1 ? 4 : 1), "%edx");
  __op::movregl(out, "%edx", "(%eax)");
  __op::addl(out, 4, "%eax");

  // Maybe initialize every string.
  if (p->type_->type->getType() == Core::STRING) {
    __op::movregl(out, "%edx", make_sp_offset(4));
    __op::movregl(out, "%eax", make_sp_offset(0));
    __op::call(out, ".stringarray_init");
    __op::movregl(out, make_sp_offset(0), "%edx");
    __op::movregl(out, "%edx", this->dest);
  } else {
    __op::movregl(out, "%eax", this->dest);
  }
};


std::string make_vtable_addr(Core::ClassType* cls) {
  std::stringstream ss;
  ss << "$.vtable." << cls->name;
  return ss.str();
};

void ExprAssembler::visitENewObj(ENewObj* p) {
  std::ostream& out = ass_env->stream;
  Core::ClassType* cls = dynamic_cast<Core::ClassType*>(p->type.get());
  assert(cls != NULL);

  int calloc_sp = align(sp - 8, 16);
  calloc_sp = call_align(calloc_sp);

  __op::movsp(out, calloc_sp);
  __op::movl(out, 1, make_sp_offset(4));
  __op::movl(out, cls->getSize() + 4, make_sp_offset(0));
  __op::call(out, "calloc");
  __op::movl(out, make_vtable_addr(cls), "(%eax)");
  out << "	addl	$" << 4 << ", %eax" << std::endl;

  std::vector<int> strings = cls->stringOffsets();
  for (int i = 0; i < strings.size(); i++) {
    __op::movl(out, strings[i], "%edx");
    __op::movregl(out, "$.emptyVal", "(%eax, %edx, 1)");
  } 

  __op::movregl(out, "%eax", this->dest);
};


void ExprAssembler::visitECast(ECast* p) {
  p->expr_->accept(this);
};


void ExprAssembler::visitENull(ENull* p) {
  __op::movl(ass_env->stream, 0, this->dest);
};


void ExprAssembler::visitEAttr(EAttr* p) {
  ExprAssembler exp_ass(scope, ass_env);

  int type_size = TypeSize::getSize(p->type.get());
  int new_sp = align(sp - type_size, type_size);
  this->arg1 = make_bp_offset(new_sp);
  exp_ass.eval(p->expr_, this->arg1, new_sp);

  OpEval(p->expr_->type, p, this);
};


void ExprAssembler::visitENewArrayGet(ENewArrayGet* p) {
  ENewArray new_arr(p->type_, p->expr_1);
  EGetArray get(&new_arr, p->expr_2);;
  get.line_number = p->line_number;
  new_arr.line_number = p->line_number;

  get.accept(this);

  new_arr.type_ = NULL;
  new_arr.expr_ = NULL;
  get.expr_1 = NULL;
  get.expr_2 = NULL;
};



// ExprAssembler }}}

}
