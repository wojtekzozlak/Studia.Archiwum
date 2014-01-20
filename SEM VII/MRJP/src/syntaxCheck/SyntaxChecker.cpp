#include <iostream>
#include "SyntaxChecker.h"
#include "Eval.h"
#include "Errors.h"
#include "parser/Printer.H"

#ifdef JVM_COMPILER
  const bool _CLASSES = false;
  const bool _ARRAYS = false;
#else
  const bool _CLASSES = true;
  const bool _ARRAYS = true;
#endif

namespace SyntaxCheck
{

// {{{ SyntaxChecker

bool parsing_successful;

std::ostream& logError(int line) {
  if (parsing_successful) {
    std::cerr << "ERROR" << std::endl;
  }
  parsing_successful = false;
  if (line > 0) std::cerr << "(line " << line << ") ";
  std::cerr << "Error: ";
  return std::cerr;
};


void SyntaxChecker::visitProgram(Program* p) {
  parsing_successful = true;

  // Pierwszy przebieg.
  ClassTreeCollector cls_collector(&env);
  p->listtopdef_->accept(&cls_collector);

  // Drugi przebieg.
  FunctionsCollector fun_collector(&env);
  p->listtopdef_->accept(&fun_collector);

  ClassBodyCollector cls_body_collector(&env);
  p->listtopdef_->accept(&cls_body_collector);


  // Trzeci przebieg
  Environment::FunMap::iterator fun_it;
  for (fun_it = env.fun.begin(); fun_it != env.fun.end(); fun_it++) {
    checkFunction(fun_it->second);
  }

  Environment::ClsMap::iterator cls_it;
  for (cls_it = env.cls.begin(); cls_it != env.cls.end(); cls_it++) {
    ClassType::PtrType cls = cls_it->second;

    ClassType::AttrMap::iterator attr_it;
    for (attr_it = cls->attrs.begin(); attr_it != cls->attrs.end(); attr_it++) {
      if (attr_it->second->getType() == FUNCTION)
        checkMethod(boost::dynamic_pointer_cast<FunType>(attr_it->second), cls);
    }
  }


  // Sprawdzenie maina.
  fun_it = env.fun.find(std::string("main"));
  if (fun_it == env.fun.end() ||
      !fun_it->second->ret_type->isA(*IntType::obj()) ||
      fun_it->second->args.size() != 0) {
    logError(0) << "Can't find entrypoint function 'int main()'" << std::endl;
  }
};


void SyntaxChecker::checkMethod(FunType::PtrType fun, ClassType::PtrType cls) {
  AbstractScope::PtrType global_scope(new GlobalScope(&env));
  AbstractScope::PtrType class_scope(new ClassScope(global_scope, cls.get()));

  // Self i argumenty metody.
  BlockScope::PtrType function_scope(new BlockScope(class_scope));
  std::string self("self");
  function_scope->define(self, boost::dynamic_pointer_cast<AbstractType>(cls));
  for (int i = 0; i < fun->args.size(); i++) {
    function_scope->define(fun->idents[i], fun->args[i]->toLValue());
  }

  BlockScope::PtrType body_scope(new BlockScope(function_scope));

  StmtChecker stmt_checker(&env, fun->ret_type, body_scope);
  Block* body_blk = (Block*) fun->body->sblock_;
  body_blk->liststmt_->accept(&stmt_checker);
};


void SyntaxChecker::checkFunction(FunType::PtrType fun) {
  if (fun->body == NULL) return ; // built-in funs

  AbstractScope::PtrType global_scope(new GlobalScope(&env));

  // Argumenty funkcji
  BlockScope::PtrType function_scope(new BlockScope(global_scope));
  for (int i = 0; i < fun->args.size(); i++) {
    function_scope->define(fun->idents[i], fun->args[i]->toLValue());
  }

  BlockScope::PtrType body_scope(new BlockScope(function_scope));

  StmtChecker stmt_checker(&env, fun->ret_type, body_scope);
  Block* body_blk = (Block*) fun->body->sblock_;
  body_blk->liststmt_->accept(&stmt_checker);

  if (fun->ret_type->getType() != VOID && !stmt_checker.reachable_return) {
    logError(fun->body->line_number) << "non void function without return" <<
      std::endl;
  }
};

// SyntaxChecker }}}



// {{{ StmtChecker

StmtChecker::StmtChecker(Environment* env, AbsPtrType ret_type,
    BlockScope::PtrType scope)
    : env(env), ret_type(ret_type), scope(scope), reachable_return(false) {};


void StmtChecker::maybePatchTree(Stmt** old) {
  if (patched.size() > 0) {
    delete *old;
    *old = patched.top();
    patched.pop();
  }
};


void StmtChecker::visitListStmt(ListStmt* p) {
  ListStmt::iterator it;
  for (it = p->begin(); it != p->end(); it++) {
    (*it)->accept(this);
    maybePatchTree(&(*it));
  }
};


void StmtChecker::visitEmpty(Empty* p) {};


void StmtChecker::visitSExp(SExp* p) {
  AbsPtrType val = ExprChecker::eval(scope, env, &p->expr_);

  if (val->getType() == FUNCTION) {
    logError(p->line_number) << "expected function object '" <<
        val->toString() << "' without call" << std::endl;
  }
};


void StmtChecker::visitRet(Ret* p) {
  AbsPtrType val = ExprChecker::eval(scope, env, &p->expr_);

  if (!val->isA(*ret_type)) {
    logError(p->line_number) << "returning '" << val->toString() <<
      "' while '" << ret_type->toString() << "' expected" << std::endl;
  }
  reachable_return = true;
};


void StmtChecker::visitVRet(VRet* p) {
  if (!VoidType::obj()->isA(*ret_type)) {
    logError(p->line_number) << "returning '" << VoidType::obj()->toString() <<
      "' while '" << ret_type->toString() << "' expected" << std::endl;
  }
  reachable_return = true;
};

void StmtChecker::visitIncr(Incr* p) {
  AbsPtrType val = ExprChecker::eval(scope, env, &p->expr_);

  if (val->getType() != INT) {
    logError(p->line_number) << "can not increment type '" << val->toString() <<
        "'" << std::endl;
  }
};


void StmtChecker::visitDecr(Decr* p) {
  AbsPtrType val = ExprChecker::eval(scope, env, &p->expr_);

  if (val->getType() != INT) {
    logError(p->line_number) << "can not decrement type '" << val->toString() <<
        "'" << std::endl;
  }
};


void StmtChecker::visitWhile(While* p) {
  AbsPtrType inv = ExprChecker::eval(scope, env, &p->expr_);

  BlockScope::PtrType new_scope(new BlockScope(scope));
  StmtChecker stmt_checker(env, ret_type, new_scope);
  p->stmt_->accept(&stmt_checker);
  stmt_checker.maybePatchTree(&p->stmt_);

  if (inv->getType() != BOOL) {
    if (inv->getType() != INVALID) {
      logError(p->expr_->line_number) << "unexpected type '" <<
          inv->toString() << "' as loop invariant" << std::endl;
    }
  }
};


void StmtChecker::visitFor(For* p) {
  AbsPtrType type = TypeCollector::eval(env, p->type_);
  AbsPtrType val = ExprChecker::eval(scope, env, &p->expr_);

  if (type->getType() == VOID) {
    logError(p->type_->line_number) << "invalid type '" << type->toString() <<
        "' as for-loop variable" << std::endl;
  }

  ArrayType::PtrType arr = boost::dynamic_pointer_cast<ArrayType>(val);
  if (type->getType() != INVALID &&
      (arr.get() == NULL || !arr->elementsType->isA(*type))) {
    logError(p->type_->line_number) << "unexpected type '" << val->toString() <<
        "' (expected '" << ArrayType(type).toString() << "')" << std::endl;
  }

  BlockScope::PtrType for_scope(new BlockScope(scope));
  if (type->getType() != INVALID && type->getType() != VOID) {
    for_scope->define(p->ident_, type->toLValue());
  }
  BlockScope::PtrType new_scope(new BlockScope(for_scope));

  StmtChecker stmt_checker(env, ret_type, new_scope);
  p->stmt_->accept(&stmt_checker);
  stmt_checker.maybePatchTree(&p->stmt_);
};


void StmtChecker::visitCondElse(CondElse *p) {
  AbsPtrType cond = ExprChecker::eval(scope, env, &p->expr_);

  BlockScope::PtrType true_scope(new BlockScope(scope));
  StmtChecker true_checker(env, ret_type, true_scope);
  p->stmt_1->accept(&true_checker);
  true_checker.maybePatchTree(&p->stmt_1);

  BlockScope::PtrType false_scope(new BlockScope(scope));
  StmtChecker false_checker(env, ret_type, false_scope);
  p->stmt_2->accept(&false_checker);
  false_checker.maybePatchTree(&p->stmt_2);

  if (cond->getType() != BOOL && cond->getType() != INVALID) {
    logError(p->line_number) << "unexpected type '" << cond->toString() <<
        "' as condition" << std::endl;
    return ;
  }

  BoolType::PtrType b = boost::dynamic_pointer_cast<BoolType>(cond);
  if (b.get() == NULL) return;

  if (!b->unknown && b->value) {
    reachable_return |= true_checker.reachable_return;

    // Remove if, left only true branch
    patched.push(p->stmt_1);
    p->stmt_1 = NULL;
  } else if (!b->unknown && !b->value) {
    reachable_return |= false_checker.reachable_return;

    // Remove if, left only false branch
    patched.push(p->stmt_2);
    p->stmt_2 = NULL;
  } else reachable_return |= (true_checker.reachable_return &&
      false_checker.reachable_return);
};


void StmtChecker::visitCond(Cond* p) {
  AbsPtrType cond = ExprChecker::eval(scope, env, &p->expr_);

  BlockScope::PtrType true_scope(new BlockScope(scope));
  StmtChecker true_checker(env, ret_type, true_scope);
  p->stmt_->accept(&true_checker);
  true_checker.maybePatchTree(&p->stmt_);

  if (cond->getType() != BOOL && cond->getType() != INVALID) {
    logError(p->line_number) << "unexpected type '" << cond->toString() <<
        "' as condition" << std::endl;
    return ;
  }

  BoolType::PtrType b = boost::dynamic_pointer_cast<BoolType>(cond);
  if (b.get() == NULL) return;

  if (!b->unknown && b->value) {
    reachable_return |= true_checker.reachable_return;

    // Remove if, left only true branch
    patched.push(p->stmt_);
    p->stmt_ = NULL;
  } else if (!b->unknown && !b->value) {
    Empty* emp = new Empty();
    patched.push(emp);
  }
};


void StmtChecker::visitDecl(Decl* p) {
  decl_buf = TypeCollector::eval(env, p->type_);
  if (decl_buf->getType() == VOID) {
    logError(p->line_number) << "can not declare variable of type '" <<
      decl_buf->toString() << "'" << std::endl;
    return ;
  } else if (decl_buf->getType() == INVALID) {
    return ;
  }
  p->listitem_->accept(this);
};


void StmtChecker::visitListItem(ListItem* p) {
  ListItem::iterator it;
  for(it = p->begin(); it != p->end(); it++) {
    (*it)->accept(this);
  }
};


void StmtChecker::visitNoInit(NoInit* p) {
  if (scope->isDefined(p->ident_)) {
    logError(p->line_number) << "variable '" << p->ident_ <<
        "' already defined in that scope" << std::endl;
  } else scope->define(p->ident_, decl_buf->toLValue());
};


void StmtChecker::visitInit(Init* p) {
  if (scope->isDefined(p->ident_)) {
    logError(p->line_number) << "variable '" << p->ident_ <<
        "' already defined in that scope" << std::endl;
  } else {
    AbsPtrType val = ExprChecker::eval(scope, env, &p->expr_);
    if (!val->isA(*decl_buf)) {
      logError(p->line_number) << "unexpected type '" << val->toString() <<
          "' for variable '" << p->ident_ << "' (expected type '" <<
          decl_buf->toString() << "')" << std::endl;
    }
    // define anyway
    scope->define(p->ident_, decl_buf->toLValue());
  }
};


void StmtChecker::visitAss(Ass* p) {
  AbsPtrType left = ExprChecker::eval(scope, env, &p->expr_1);
  AbsPtrType right = ExprChecker::eval(scope, env, &p->expr_2);

  if (!left->lvalue)
    logError(p->line_number) << "can not assign to rvalue" << std::endl;
  else if (!right->isA(*left)) {
    logError(p->line_number) << "can not assign value of type '" <<
        right->toString() << "' to variable of type '" << left->toString() <<
        "'" << std::endl;
  }
};


void StmtChecker::visitBStmt(BStmt* p) {
  p->sblock_->accept(this);
};


void StmtChecker::visitBlock(Block *p) {
  BlockScope::PtrType new_scope(new BlockScope(scope));
  StmtChecker stmt_checker(env, ret_type, new_scope);
  p->liststmt_->accept(&stmt_checker);

  reachable_return |= stmt_checker.reachable_return;
};

// StmtChecker }}}



// {{{ ExprChecker

AbsPtrType ExprChecker::eval(BlockScope::PtrType scope, Environment* env,
    Expr** expr_p) {
  ExprChecker expr_checker(scope, env);
  expr_checker.expr_p = expr_p;
  (*expr_p)->accept(&expr_checker);
  (*expr_p)->type = expr_checker.get();
  maybeFoldConstants(expr_p);
  return expr_checker.get();
}


ExprChecker::ExprChecker(BlockScope::PtrType scope, Environment* env)
    : scope(scope), env(env) {};


AbsPtrType ExprChecker::get() {
  assert(eval_stack.size() == 1);
  return eval_stack.top();
};


void ExprChecker::checkExprError(Expr *p, AbsPtrType& res) {
  if (!expr_failure && res->getType() == INVALID) {
    InvalidType *inv = static_cast<InvalidType*>(res.get());
    expr_failure = true;
    logError(p->line_number) << inv->reason << std::endl;
  }
};


// Type objects construction

void ExprChecker::visitEVar(EVar* p) {
  AbstractScope::EnumeratedVar e_var = scope->get(p->ident_);

  AbsPtrType val = e_var.second;
  if (val.get() == NULL) {
    logError(p->line_number) << "undefined variable '" << p->ident_ << "'" <<
        std::endl;
    eval_stack.push(InvalidType::obj()->toRValue());
  } else {
    if (e_var.first == FIELD_VAR || e_var.first == METHOD_VAR) {
      EVar* var = new EVar("self"); // TODO : fix that microleak?
      var->type = scope->get("self").second;
      EAttr* attr = new EAttr(var, p->ident_);
      attr->type = val;
      *this->expr_p = attr;
    }
 
    eval_stack.push(val);
  }
};


void ExprChecker::visitENewObj(ENewObj* p) {
  AbsPtrType val = TypeCollector::eval(env, p->type_);

  if (val->getType() == INVALID) {
    expr_failure = true;
    eval_stack.push(InvalidType::obj());
  } else if (val->getType() != CLASS) {
    logError(p->type_->line_number) <<
        "can not create instance of non-class type '" << val->toString() <<
        "'" << std::endl;
    eval_stack.push(InvalidType::obj());
  } else eval_stack.push(val->toRValue());
};


void ExprChecker::visitENewArray(ENewArray* p) {
  AbsPtrType val = TypeCollector::eval(env, p->type_);
  AbsPtrType len = ExprChecker::eval(scope, env, &p->expr_);

  if (val->getType() == INVALID || val->getType() == VOID) {
    if (val->getType() == VOID) {
      logError(p->type_->line_number) << "can not create array of type '" <<
          val->toString() << "'" << std::endl;
    }
    expr_failure = true;
    eval_stack.push(InvalidType::obj());
  } else if (len->getType() != INT) {
    logError(p->line_number) << "unexpected type '" << len->toString() <<
        "' as an array length" << std::endl;
    expr_failure = true;
    eval_stack.push(ArrayType(val).toRValue());
  } else eval_stack.push(ArrayType(val).toRValue());
};


void ExprChecker::visitENewArrayGet(ENewArrayGet* p) {
  ENewArray new_arr(p->type_, p->expr_1);
  EGetArray get(&new_arr, p->expr_2);;
  get.line_number = p->line_number;
  new_arr.line_number = p->line_number;

  get.accept(this);
  logError(p->line_number) << "can't subscribe to a freshly created array" << std::endl;

  new_arr.type_ = NULL;
  new_arr.expr_ = NULL;
  get.expr_1 = NULL;
  get.expr_2 = NULL;
};


void ExprChecker::visitEGetArray(EGetArray* p) {
  AbsPtrType array = ExprChecker::eval(scope, env, &p->expr_1);
  AbsPtrType ix = ExprChecker::eval(scope, env, &p->expr_2);

  if (array->getType() != ARRAY) {
    logError(p->line_number) << "getting index of non-array type" << std::endl;
    expr_failure = true;
    eval_stack.push(InvalidType::obj());
    return ;
  }
  if (ix->getType() != INT) {
    logError(p->line_number) << "non integer index" << std::endl;
    expr_failure = true;
  }

  ArrayType* ret = dynamic_cast<ArrayType*>(array.get());
  assert(ret != NULL);
  eval_stack.push(ret->elementsType->toLValue());
};


void ExprChecker::visitECast(ECast* p) {
  AbsPtrType type = TypeCollector::eval(env, p->type_);
  AbsPtrType val = ExprChecker::eval(scope, env, &p->expr_);

  if (type->getType() != CLASS) {
    logError(p->line_number) << "cast to non-class type" << std::endl;
    expr_failure = true;
    eval_stack.push(InvalidType::obj());
    return ;
  }
  if (!((val->getType() == CLASS && (val->isA(*type) || type->isA(*val))) ||
      val->getType() == NULLY)){
    logError(p->line_number) << "casting to type '" << type->toString() <<
        "' from incompatible type '" << val->toString() << "'" << std::endl;
  }

  eval_stack.push(type->toRValue());
};


void ExprChecker::visitEAttr(EAttr* p) {
  AbsPtrType cls = ExprChecker::eval(scope, env, &p->expr_);

  if (cls->getType() == ARRAY) {
    if (p->ident_.compare(String("length")) != 0) {
      logError(p->line_number) << "getting attribute '" << p->ident_ <<
          "' of type '" << cls->toString() << "'" << std::endl;
    }
    ArrayType::PtrType arr = boost::dynamic_pointer_cast<ArrayType>(cls);
    eval_stack.push(IntType::obj()->toRValue());
    return ;
  }

  if (cls->getType() != CLASS) {
    logError(p->line_number) << "getting attribute of non class type '" <<
        cls->toString() << "'" << std::endl;
    expr_failure = true;
    eval_stack.push(InvalidType::obj());
    return ;
  }

  ClassType* cls_ptr = dynamic_cast<ClassType*>(cls.get());
  assert(cls_ptr != NULL);
  AbsPtrType attr = cls_ptr->getAttr(p->ident_);
  if (attr.get() == NULL) {
    logError(p->line_number) << "unknown attribute '" << p->ident_ <<
        "' of class '" << cls->toString() << "'" << std::endl;
    expr_failure = true;
    eval_stack.push(InvalidType::obj());
  } else if(attr->getType() != FUNCTION) eval_stack.push(attr->toLValue());
  else eval_stack.push(attr);
};


void ExprChecker::visitEApp(EApp* p) {
  AbsPtrType val = ExprChecker::eval(scope, env, &p->expr_);

  if (val->getType() != FUNCTION) {
    if (val->getType() != INVALID) {
      logError(p->line_number) << "calling non-function type '" <<
          val->toString() << "'" << std::endl;
    }
    expr_failure = true;
    eval_stack.push(InvalidType::obj());
    return ;
  }

  FunType::PtrType fun = boost::dynamic_pointer_cast<FunType>(val);
  if (p->listexpr_->size() != fun->args.size()) {
    logError(p->line_number) << "wrong number of arguments (" <<
        p->listexpr_->size() << ") for function '" << fun->name << "'" <<
        std::endl;
  } else {
    for(int i = 0; i < p->listexpr_->size(); i++) {
      AbsPtrType attr = ExprChecker::eval(scope, env, &((*p->listexpr_)[i]));
      if (!attr->isA(*fun->args[i]) && attr->getType() != INVALID) {
        logError((*p->listexpr_)[i]->line_number) << "wrong parameter " <<
            i << " for function '" << fun->name << "' of type '" <<
            attr->toString() << "' (expected '" << fun->args[i]->toString() <<
            "')" << std::endl; 
      }
    }
  }

  EVar* var_node = dynamic_cast<EVar*>(p->expr_);
  if (fun->cls.get() != NULL && var_node != NULL) {
    EAttr* attr = new EAttr(var_node, var_node->ident_);
    attr->type = fun;
    var_node->ident_ = "self";
    var_node->type = scope->get("self").second;
    p->expr_ = attr;
  }

  eval_stack.push(fun->ret_type->toRValue());
};


void ExprChecker::visitELitInt(ELitInt* p) {
  eval_stack.push(AbstractType::PtrType(new IntType(p->integer_)));
};


void ExprChecker::visitELitTrue(ELitTrue* p) {
  eval_stack.push(AbstractType::PtrType(new BoolType(true)));
};


void ExprChecker::visitELitFalse(ELitFalse* p) {
  eval_stack.push(AbstractType::PtrType(new BoolType(false)));
};


void ExprChecker::visitENull(ENull* p) {
  eval_stack.push(NullType::obj());
};


void ExprChecker::visitEString(EString* p) {
  eval_stack.push(StringType::obj());
};


// Math / logical operators

void ExprChecker::checkUnaryExpr(Expr* p, Expr** arg) {
  AbstractType::PtrType val, res;

  Expr** expr_p_bak = this->expr_p;

  this->expr_p = arg;
  (*arg)->accept(this);
  val = eval_stack.top();
  (*arg)->type = val;
  eval_stack.pop();

  res = UnaryOpEval(val, p).result;
  eval_stack.push(res);
  this->expr_p = expr_p_bak;

  checkExprError(p, res);
};


void ExprChecker::checkBinaryExpr(Expr *p, Visitable *op, Expr** arg1,
    Expr** arg2) {
  AbstractType::PtrType val1, val2, res;

  Expr** expr_p_bak = this->expr_p;


  this->expr_p = arg1;
  (*arg1)->accept(this);
  val1 = eval_stack.top();
  (*arg1)->type = val1;
  maybeFoldConstants(arg1);
  eval_stack.pop();

  this->expr_p = arg2;
  (*arg2)->accept(this);
  val2 = eval_stack.top();
  (*arg2)->type = val2;
  maybeFoldConstants(arg2);
  eval_stack.pop();

  res = BinaryOpEval(val1, val2, op).result;
  eval_stack.push(res);

  this->expr_p = expr_p_bak;
  checkExprError(p, res);
}


void ExprChecker::maybeFoldConstants(Expr** p) {
  Expr* e = *p;
  TypeType type = e->type->getType();
  if (type == INT) {
    IntType* i = dynamic_cast<IntType*>(e->type.get());
    assert(i != NULL);
    if (!i->unknown) {
      ELitInt* lit_int = new ELitInt(i->value);
      lit_int->type = e->type;
      *p = lit_int;
      delete e;
    }
  } else if(type == BOOL) {
    BoolType* b = dynamic_cast<BoolType*>(e->type.get());
    assert(b != NULL);
    if (!b->unknown) {
      if (b->value)
        *p = new ELitTrue();
      else
        *p = new ELitFalse();
      (*p)->type = e->type;
      delete e;
    }
  }
}


void ExprChecker::visitNeg(Neg* p) {
  checkUnaryExpr(p, &p->expr_);
};


void ExprChecker::visitNot(Not* p) {
  checkUnaryExpr(p, &p->expr_);
};


void ExprChecker::visitEMul(EMul* p) {
  checkBinaryExpr(p, p->mulop_, &p->expr_1, &p->expr_2);
};


void ExprChecker::visitEAdd(EAdd* p) {
  checkBinaryExpr(p, p->addop_, &p->expr_1, &p->expr_2);
};


void ExprChecker::visitERel(ERel* p) {
  checkBinaryExpr(p, p->relop_, &p->expr_1, &p->expr_2);
};


void ExprChecker::visitEAnd(EAnd* p) {
  checkBinaryExpr(p, p, &p->expr_1, &p->expr_2);
};


void ExprChecker::visitEOr(EOr* p) {
  checkBinaryExpr(p, p, &p->expr_1, &p->expr_2);
};

// ExprChecker }}}


// {{{ ClassTreeCollector

ClassTreeCollector::ClassTreeCollector(Environment* env) : env(env) {};


void ClassTreeCollector::visitListTopDef(ListTopDef *p) {
  ListTopDef::iterator it;
  for (it = p->begin(); it != p->end(); it++) {
    (*it)->accept(this);
  }
};


void ClassTreeCollector::visitFnDef(FnDef* p) {};


void ClassTreeCollector::visitClsDef(ClsDef* p) {
  if (!_CLASSES) {
    logError(p->line_number) << "Can not define class in current mode." <<
        std::endl;
    return ;
  }

  ClassType::PtrType cls(new ClassType(p->ident_));
  Environment::ClsMap::iterator it;
  bool good = true;

  // Free name?
  it = env->cls.find(p->ident_);
  if (it != env->cls.end()) {
    good = false;
    logError(p->line_number) << p->ident_ << " already exists" << std::endl;
  }

  // Extension?
  ClsExt* ext = dynamic_cast<ClsExt*>(p->ext_);
  if (ext != NULL) {
    it = env->cls.find(ext->ident_);
    if (it != env->cls.end()) cls->parent = it->second;
    else {
      good = false;
      logError(p->line_number) << "extending unknown class " << ext->ident_ <<
          std::endl;
    }
  }

  if (good) env->cls.insert(make_pair(p->ident_, cls));
};

// ClassTreeCollector }}}

// {{{ ClassBodyCollector

ClassBodyCollector::ClassBodyCollector(Environment* env)
    : ClassTreeCollector(env) {};

void ClassBodyCollector::visitClsDef(ClsDef* p) {
  // Existence check
  Environment::ClsMap::iterator it;
  it = env->cls.find(p->ident_);
  if (it == env->cls.end()) return ;
  cls_buf = it->second;

  p->listindef_->accept(this);
};


void ClassBodyCollector::visitListInDef(ListInDef* p) {
  ListInDef::iterator it;
  for (it = p->begin(); it != p->end(); it++) {
    (*it)->accept(this);
  }
};


void ClassBodyCollector::visitMethDef(MethDef* p) {
  FunctionCollector fun_collector(env);
  p->function_->accept(&fun_collector);

  FunType::PtrType fun = fun_collector.get();
  fun->cls = cls_buf;
  AbsPtrType current = cls_buf->getAttr(fun->name);

  if (!fun_collector.good)
   return ;

  if (current.get() != NULL) {
    if (current->getType() != FUNCTION) {
      logError(p->line_number) << "can not override attribute '" << fun->name
          << "'" << std::endl;
        return ;
    } else if (!current->isA(*fun)) {
      logError(p->line_number) << "can not override function '" <<
           current->toString() << "' with function '" << fun->toString() <<
          "'" << std::endl;
      return ;
    }
  }
  AbsPtrType attr = boost::static_pointer_cast<AbstractType>(fun);
  cls_buf->attrs.insert(std::make_pair(fun->name, attr));
};


void ClassBodyCollector::visitAtrDef(AtrDef* p) {
  AbsPtrType type = TypeCollector::eval(env, p->type_);

  ListIdent::iterator it;
  for (it = p->listident_->begin(); it != p->listident_->end(); it++) {
    AbsPtrType current = cls_buf->getAttr(*it);
    if (type->getType() == INVALID) {
      continue ;
    } else if (current.get() != NULL) {
      logError(p->line_number) << "can not override a member '" << *it <<
          "' with a new attribute" << std::endl;
    } else cls_buf->attrs.insert(std::make_pair(*it, type));
  }
};

// ClassBodyCollector }}}


// {{{ FunctionsCollector

FunctionsCollector::FunctionsCollector(Environment* env) : env(env) {};

void FunctionsCollector::visitListTopDef(ListTopDef *p) {
  ListTopDef::iterator it;
  for (it = p->begin(); it != p->end(); it++) {
    (*it)->accept(this);
  }
};


void FunctionsCollector::visitFnDef(FnDef* p) {
  FunctionCollector fun_collector(env);
  p->function_->accept(&fun_collector);
  FunType::PtrType fun = fun_collector.get();


  if (env->fun.find(fun->name) != env->fun.end()) {
    fun_collector.good = false;
    logError(p->line_number) << "function '" << fun->name
        << "' already exists" << std::endl;
  }

  if (fun_collector.good) env->insertFunction(fun);
};


void FunctionsCollector::visitClsDef(ClsDef *p) {};

// FunctionsCollector }}}


// {{{ FunctionCollector

FunctionCollector::FunctionCollector(Environment* env)
    : env(env), good(false) {};


FunType::PtrType FunctionCollector::get() {
  assert(fun.get() != NULL);
  return fun;
};

void FunctionCollector::visitFun(Fun *p) {
  fun = FunType::PtrType(new FunType(p->ident_));
  fun->body = p;
  TypeCollector type_collector(env);
  good = true;

  // return value
  p->type_->accept(&type_collector);
  AbsPtrType ret_type = type_collector.get();
  if (ret_type->getType() == INVALID) good = false;
  else fun->ret_type = ret_type;

  // args
  p->listfarg_->accept(this);
  assert(fun->args.size() == fun->idents.size());
  std::vector<AbsPtrType>::iterator it;
  for (it = fun->args.begin(); it != fun->args.end(); it++) {
    TypeType arg_type = (*it)->getType();
    if (arg_type == INVALID) good = false;
    else if (arg_type == VOID) {
      good = false;
      logError(p->line_number) << "type '" << (*it)->toString()
          << "' can't be a function argument" << std::endl;
    }
  }
};


void FunctionCollector::visitListFArg(ListFArg* p) {
  ListFArg::iterator it;
  for (it = p->begin(); it != p->end(); it++) {
    (*it)->accept(this);
  }
};


void FunctionCollector::visitArg(Arg* p) {
  if (names_buf.find(p->ident_) != names_buf.end()) {
    logError(p->line_number) << "repeated argument identifier " << p->ident_ <<
        std::endl;
  }

  fun->args.push_back(TypeCollector::eval(env, p->type_));
  fun->idents.push_back(p->ident_);
  names_buf.insert(p->ident_);
};

// FunctionCollector }}}


// {{{ TypeCollector

AbsPtrType TypeCollector::eval(Environment* env, Type* t) {
  TypeCollector type_collector(env);
  t->accept(&type_collector);
  t->type = type_collector.get();
  return type_collector.get();
}


TypeCollector::TypeCollector(Environment *env) : env(env) {};


AbsPtrType TypeCollector::get() {
  assert(val.get() != NULL);
  return val;
};

void TypeCollector::visitInt(Int* p) {
  val = IntType::obj();
};


void TypeCollector::visitStr(Str* p) {
  val = StringType::obj();
};


void TypeCollector::visitBool(Bool* p) {
  val = BoolType::obj();
};


void TypeCollector::visitVoid(Void* p) {
  val = VoidType::obj();
};


void TypeCollector::visitArray(Array* p) {
  if (!_ARRAYS) {
    logError(p->line_number) << "Can not define arrays in current mode." <<
        std::endl;
  }
  AbsPtrType type = TypeCollector::eval(env, p->type_);

  if (type->getType() == INVALID) val = InvalidType::obj();
  else if(type->getType() == VOID || type->getType() == ARRAY) {
    val = InvalidType::obj();
    logError(p->line_number) << "array of type '" << type->toString() <<
        "' is incorrect" << std::endl;
  } else val = AbsPtrType(new ArrayType(type));
};


void TypeCollector::visitClass(Class* p) {
  Environment::ClsMap::iterator it = env->cls.find(p->ident_);
  if (it == env->cls.end()) {
    logError(p->line_number) << "unknown class " << "'" << p->ident_ << "'" <<
        std::endl;
    val = InvalidType::obj();
  } else val = it->second;
};

// TypeCollector }}}

}
