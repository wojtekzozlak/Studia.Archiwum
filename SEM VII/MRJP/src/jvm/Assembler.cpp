#include <sstream>
#include "Assembler.h"
#include "Eval.h"
#include "BuiltIn.h"
#include "syntaxCheck/SyntaxChecker.h"

namespace JVM {

static std::string global_class;


void instBool(std::ostream& stream, LabelPair& tf, std::string& e) {
  stream << tf.first << ":" << std::endl;
  stream << "  iconst_1" << std::endl;
  stream << "  goto " << e << std::endl;
  stream << tf.second << ":" << std::endl;
  stream << "  iconst_0" << std::endl;
  stream << e << ":" << std::endl;
};


void pushInt(int i, std::ostream& stream) {
  if (0 <= i && i <= 5) {
    stream << "  iconst_";
  } else if(-128 <= i && i <= 127) {
    stream << "  bipush ";
  } else if(-32768 <= i && i <= 32767) {
    stream << "  sipush ";
  } else {
    stream << "  ldc ";
  }

  stream << i << std::endl;
};


// {{{ Assembler

Assembler::Assembler(Core::Environment* env, std::string& name) : env(env),
    name(name) {}


void Assembler::assembly() {
  writeBuiltIns(std::cout, name);
  global_class = name;

  Core::Environment::FunMap::iterator fun_it;
  for (fun_it = env->fun.begin(); fun_it != env->fun.end(); fun_it++) {
    assemblyFunction(fun_it->second, env);
  }
};


std::string Assembler::functionSignature(Core::FunType::PtrType fun) {
  std::ostringstream ss;
  ss << fun->name << "(";

  for (int i = 0; i < fun->args.size(); i++) {
    ss << TypeSignature(fun->args[i]).get();
  }
  ss << ")";
  ss << TypeSignature(fun->ret_type).get();

  return ss.str();
};


void Assembler::assemblyFunction(Core::FunType::PtrType fun,
    Core::Environment* env) {
  if (fun->body == NULL) return;

  std::ostringstream ss;

  Core::AbstractScope::PtrType global_scope(new Core::GlobalScope(env));

  Core::BlockScope::PtrType function_scope(new Core::BlockScope(global_scope));
  for (int i = 0; i < fun->args.size(); i++) {
    function_scope->define(fun->idents[i], fun->args[i]->toLValue());
  }

  Core::BlockScope::PtrType body_scope(new Core::BlockScope(function_scope));



  std::cout << "; " << fun->name << std::endl;
  std::cout << ".method public static " << functionSignature(fun) << std::endl;

  StmtAssembler ass(body_scope, env, ss);
  fun->body->sblock_->accept(&ass);

  std::cout << ".limit stack " << ass.stackLimit << std::endl;
  std::cout << ".limit locals " << global_scope->maxIx() << std::endl;
  std::cout << ss.str();

  if (fun->ret_type->getType() == Core::VOID)
    std::cout << "  return" << std::endl;

  std::cout << ".end method" << std::endl << std::endl;
};

// Assembler }}}


// {{{ TypeSignature

TypeSignature::TypeSignature(Core::AbstractType::PtrType t) {
  t->accept(this);
};


std::string TypeSignature::get() {
  return str;
};

void TypeSignature::visitInt(Core::IntType* t) { str = "I"; };


void TypeSignature::visitVoid(Core::VoidType* t) { str = "V"; };


void TypeSignature::visitString(Core::StringType* t) {
  str = "Ljava/lang/String;";
};


void TypeSignature::visitArray(Core::ArrayType* t) {
  std::ostringstream ss;
  ss << "[" << TypeSignature(t->elementsType).get();
  str = ss.str();
};


void TypeSignature::visitClass(Core::ClassType* t) {
  std::ostringstream ss;
  ss << "L" << t->name << ";";
  str = ss.str();
};


void TypeSignature::visitBool(Core::BoolType* t) {
  str = "I";
};

// TypeSignature }}}


// {{{ StmtAssembler

std::string makeLabel(const char* lab, int ix) {
  std::ostringstream ss;
  ss << lab << "_" << ix;
  return ss.str();
};

bool isReturn(Visitable* op) {
  Ret* ret = dynamic_cast<Ret*>(op);
  return ret != NULL;
};

StmtAssembler::StmtAssembler(Core::BlockScope::PtrType scope,
    Core::Environment* env, std::ostream& stream)
    : scope(scope), env(env), stream(stream), stackLimit(0) {};


void StmtAssembler::visitListStmt(ListStmt* p) {
  ListStmt::iterator it;
  for (it = p->begin(); it != p->end(); it++) {
    (*it)->accept(this);
    // TODO leak;
    if (isReturn(*it)) {
      it++;
      p->erase(it, p->end());
      break;
    }
  }
};


void StmtAssembler::visitBlock(Block* p) {
  Core::BlockScope::PtrType new_scope(new Core::BlockScope(scope));
  StmtAssembler stmt_ass(new_scope, env, stream);
  p->liststmt_->accept(&stmt_ass);

  stackLimit = std::max(stackLimit, stmt_ass.stackLimit);
};


void StmtAssembler::visitSExp(SExp* p) {
  ExprAssembler expr_ass(scope, env, stream);
  expr_ass.eval(p->expr_);

  if (expr_ass.evalStack.top()->getType() != Core::VOID)
    stream << "  pop" << std::endl;
  else stream << "  nop" << std::endl;
  expr_ass.evalStack.pop();

  stackLimit = std::max(stackLimit, expr_ass.stackLimit);
};


void StmtAssembler::visitRet(Ret* p) {
  ExprAssembler expr_ass(scope, env, stream);
  expr_ass.eval(p->expr_);
  stackLimit = std::max(stackLimit, expr_ass.stackLimit);

  UnaryOpEval(expr_ass.evalStack.top(), p, &expr_ass);
};


void StmtAssembler::visitVRet(VRet* p) {
  stream << "  return" << std::endl;
};


void StmtAssembler::visitDecl(Decl* p) {
  declBuf = SyntaxCheck::TypeCollector::eval(env, p->type_);

  ListItem::iterator it;
  for (it = p->listitem_->begin(); it != p->listitem_->end(); it++) {
    (*it)->accept(this);
  };
};


void StmtAssembler::visitNoInit(NoInit* p) {
  ExprAssembler expr_ass(scope, env, stream);
  UnaryOpEval(declBuf, p, &expr_ass);
  stackLimit = std::max(stackLimit, (size_t)1);
};


void StmtAssembler::visitInit(Init* p) {
  ExprAssembler expr_ass(scope, env, stream);
  expr_ass.push(declBuf);
  expr_ass.eval(p->expr_);

  UnaryOpEval(declBuf, p, &expr_ass);
  stackLimit = std::max(stackLimit, expr_ass.stackLimit);
}


void StmtAssembler::visitCond(Cond* p) {
  LabelPair labels;
  int ix = scope->nextIx();
  labels.first = makeLabel("Cond_true", ix);
  labels.second = makeLabel("Cond_false", ix);

  ExprAssembler expr_ass(scope, env, stream);
  expr_ass.labelStack.push(&labels);
  p->expr_->accept(&expr_ass);
  stackLimit = std::max(stackLimit, expr_ass.stackLimit);

  stream << labels.first << ":" << std::endl;

  Core::BlockScope::PtrType true_scope(new Core::BlockScope(scope));
  StmtAssembler true_checker(true_scope, env, stream);
  p->stmt_->accept(&true_checker);
  stackLimit = std::max(stackLimit, true_checker.stackLimit);

  stream << labels.second << ":" << std::endl;
};


void StmtAssembler::visitCondElse(CondElse* p) {
  LabelPair labels;
  int ix = scope->nextIx();
  labels.first = makeLabel("CondElse_true", ix);
  labels.second = makeLabel("CondElse_false", ix);
  std::string end_label = makeLabel("CondElse_end", ix);

  ExprAssembler expr_ass(scope, env, stream);
  expr_ass.labelStack.push(&labels);
  p->expr_->accept(&expr_ass);
  stackLimit = std::max(stackLimit, expr_ass.stackLimit);

  stream << labels.first << ":" << std::endl;

  Core::BlockScope::PtrType true_scope(new Core::BlockScope(scope));
  StmtAssembler true_checker(true_scope, env, stream);
  p->stmt_1->accept(&true_checker);
  stackLimit = std::max(stackLimit, true_checker.stackLimit);

  stream << "  goto " << end_label << std::endl;
  stream << labels.second << ":" << std::endl;

  Core::BlockScope::PtrType false_scope(new Core::BlockScope(scope));
  StmtAssembler false_checker(false_scope, env, stream);
  p->stmt_2->accept(&false_checker);
  stackLimit = std::max(stackLimit, false_checker.stackLimit);

  stream << end_label << ":" << std::endl;
  stream << "  nop" << std::endl;
};


void StmtAssembler::visitBStmt(BStmt* p) {
  p->sblock_->accept(this);
};


void StmtAssembler::visitAss(Ass* p) {
  EVar* var_node = dynamic_cast<EVar*>(p->expr_1);
  assert(var_node != NULL);

  ExprAssembler expr_ass(scope, env, stream);
  p->expr_2->accept(&expr_ass);
  stackLimit = std::max(stackLimit, expr_ass.stackLimit);

  const Core::AbstractScope::EnumeratedVar& var = scope->get(var_node->ident_);
  if (var.second->getType() == Core::INT ||
      var.second->getType() == Core::BOOL) {
    stream << "  istore " << var.first << std::endl;
  } else if (var.second->getType() == Core::STRING) {
    stream << "  astore " << var.first << std::endl;
  } else assert(false);
};


void StmtAssembler::visitWhile(While* p) {
  LabelPair labels;
  int ix = scope->nextIx();
  labels.first = makeLabel("While_body", ix);
  labels.second = makeLabel("While_end", ix);
  std::string cond_label = makeLabel("While_cond", ix);

  stream << cond_label << ":" << std::endl;

  ExprAssembler expr_ass(scope, env, stream);
  expr_ass.labelStack.push(&labels);

  p->expr_->accept(&expr_ass);
  stackLimit = std::max(stackLimit, expr_ass.stackLimit);

  stream << labels.first << ":" << std::endl;

  Core::BlockScope::PtrType new_scope(new Core::BlockScope(scope));
  StmtAssembler body_ass(new_scope, env, stream);
  p->stmt_->accept(&body_ass);
  stackLimit = std::max(stackLimit, body_ass.stackLimit);

  stream << "  goto " << cond_label << std::endl;
  stream << labels.second << ":" << std::endl;

};


void StmtAssembler::visitDecr(Decr* p) {
  int ix = scope->get(p->ident_).first;
  stream << "  iinc " << ix << " -1" << std::endl;
}


void StmtAssembler::visitIncr(Incr* p) {
  int ix = scope->get(p->ident_).first;
  stream << "  iinc " << ix << " 1" << std::endl;
};


void StmtAssembler::visitEmpty(Empty* p) {};

// StmtAssembler }}}


// {{{ ExprAssembler

ExprAssembler::ExprAssembler(Core::BlockScope::PtrType scope,
    Core::Environment* env, std::ostream& stream) : scope(scope),
    env(env), stackLimit(0), stream(stream) {};


void ExprAssembler::push(Core::AbstractType::PtrType t) {
  evalStack.push(t);
  stackLimit = std::max(stackLimit, evalStack.size());
};


LabelPair* ExprAssembler::nextLabelPair() {
  assert(labelStack.size() > 0);
  LabelPair* lab = labelStack.top();
  labelStack.pop();
  return lab;
};


void ExprAssembler::eval(Expr* e) {
  LabelPair labels;
  int ix = scope->nextIx();
  labels.first = makeLabel("ExpVal_true", ix);
  labels.second = makeLabel("ExpVal_false", ix);
  std::string end_lab = makeLabel("ExpVal_end", ix);

  labelStack.push(&labels);
  e->accept(this);
  if (labelStack.size() == 0 || labelStack.top() != &labels) {
    instBool(stream, labels, end_lab);
  } else labelStack.pop();
};


void ExprAssembler::visitELitTrue(ELitTrue* p) {
  LabelPair* labels = nextLabelPair();
  stream << "  goto " << labels->first << std::endl;
  push(Core::BoolType::obj());
};


void ExprAssembler::visitELitFalse(ELitFalse* p) {
  LabelPair* labels = nextLabelPair();
  stream << "  goto " << labels->second << std::endl;
  push(Core::BoolType::obj());
};


void ExprAssembler::visitELitInt(ELitInt* p) {
  pushInt(p->integer_, stream);
  push(Core::IntType::obj());
};


void ExprAssembler::visitEString(EString* p) {
  stream << "  ldc \"" << p->string_ << "\"" << std::endl;
  push(Core::StringType::obj());
};


void ExprAssembler::visitNeg(Neg* p) {
  unaryExpr(p->expr_, p);
};

void ExprAssembler::visitNot(Not* p) {
  LabelPair* labels = nextLabelPair();
  LabelPair not_labels;
  not_labels.first = labels->second;
  not_labels.second = labels->first;

  labelStack.push(&not_labels);
  unaryExpr(p->expr_, p);
};


void ExprAssembler::visitEMul(EMul* p) {
  binaryExpr(p->expr_1, p->expr_2, p->mulop_);
};


void ExprAssembler::unaryExpr(Visitable* arg, Visitable* op) {
  Core::AbstractType::PtrType v;

  arg->accept(this);
  v = evalStack.top();
  evalStack.pop();

  push(UnaryOpEval(v, op, this).result);
};


void ExprAssembler::binaryExpr(Visitable* arg1, Visitable* arg2, Visitable* op) {
  Core::AbstractType::PtrType v1, v2;

  arg1->accept(this);
  arg2->accept(this);
  v2 = evalStack.top();
  evalStack.pop();
  v1 = evalStack.top();
  evalStack.pop();

  push(BinaryOpEval(v1, v2, op, this).result);
};


void ExprAssembler::visitERel(ERel* p) {
  LabelPair labels_b1, labels_b2;
  std::string b1_end, b2_end;
  int ix1 = scope->nextIx(), ix2 = scope->nextIx();

  labels_b1.first = makeLabel("B1_true", ix1);
  labels_b1.second = makeLabel("B1_false", ix1);
  b1_end = makeLabel("B1_end", ix1);

  labels_b2.first = makeLabel("B2_true", ix2);
  labels_b2.second = makeLabel("B2_false", ix2);
  b2_end = makeLabel("B2_end", ix2);

  Core::AbstractType::PtrType v1, v2;

  labelStack.push(&labels_b1);
  p->expr_1->accept(this);
  if (labelStack.size() == 0 || labelStack.top() != &labels_b1)
    instBool(stream, labels_b1, b1_end);
  else labelStack.pop();


  labelStack.push(&labels_b2);
  p->expr_2->accept(this);
  if (labelStack.size() == 0 || labelStack.top() != &labels_b2)
    instBool(stream, labels_b2, b2_end);
  else labelStack.pop();

  v2 = evalStack.top();
  evalStack.pop();
  v1 = evalStack.top();
  evalStack.pop();

  push(BinaryOpEval(v1, v2, p->relop_, this).result);
};


void ExprAssembler::visitEAnd(EAnd* p) {
  LabelPair* target = nextLabelPair();
  LabelPair lab_e1, lab_e2;
  std::string e2_comp;
  int ix = scope->nextIx();

  e2_comp = makeLabel("EAND_snd", ix);
  lab_e1.first = e2_comp;
  lab_e1.second = target->second;
  lab_e2.first = target->first;
  lab_e2.second = target->second;

  labelStack.push(&lab_e2);
  labelStack.push(&lab_e1);
  p->expr_1->accept(this);
  stream << e2_comp << ":" << std::endl;
  p->expr_2->accept(this);


  Core::AbstractType::PtrType v1, v2;
  v2 = evalStack.top();
  evalStack.pop();
  v1 = evalStack.top();
  evalStack.pop();

  push(BinaryOpEval(v1, v2, p, this).result);
};


void ExprAssembler::visitEOr(EOr* p) {
  LabelPair* target = nextLabelPair();
  LabelPair lab_e1, lab_e2;
  std::string e2_comp;
  int ix = scope->nextIx();

  e2_comp = makeLabel("EOR_snd", ix);
  lab_e1.first = target->first;
  lab_e1.second = e2_comp;
  lab_e2.first = target->first;
  lab_e2.second = target->second;

  labelStack.push(&lab_e2);
  labelStack.push(&lab_e1);
  p->expr_1->accept(this);
  stream << e2_comp << ":" << std::endl;
  p->expr_2->accept(this);


  Core::AbstractType::PtrType v1, v2;
  v2 = evalStack.top();
  evalStack.pop();
  v1 = evalStack.top();
  evalStack.pop();
  push(BinaryOpEval(v1, v2, p, this).result);
};

void ExprAssembler::visitEAdd(EAdd* p) {
  binaryExpr(p->expr_1, p->expr_2, p->addop_);
}


void ExprAssembler::visitEVar(EVar* p) {
  const Core::AbstractScope::EnumeratedVar& var = scope->get(p->ident_);

  if (var.first == Core::FUNCTION_VAR) {
    evalStack.push(var.second);
  } else if (var.first >= 0)
    push(UnaryOpEval(var.second, p, this).result);
  else assert(false);
};


void ExprAssembler::visitEApp(EApp* p) {
  p->expr_->accept(this);

  Core::FunType::PtrType fun = boost::dynamic_pointer_cast<Core::FunType>(
      evalStack.top());
  assert(fun.get() != NULL);
  evalStack.pop();

  ListExpr::iterator it;
  for (it = p->listexpr_->begin(); it != p->listexpr_->end(); it++) {
    eval(*it);
  }
  stream << "  invokestatic " << global_class << "/" <<
      Assembler::functionSignature(fun) << std::endl;

  for (it = p->listexpr_->begin(); it != p->listexpr_->end(); it++) {
    evalStack.pop();
  }

  if (fun->ret_type->getType() == Core::BOOL) {
    LabelPair* labels = nextLabelPair();
    stream << "  ifeq " << labels->second << std::endl;
    stream << "  goto " << labels->first << std::endl;
  }

  push(fun->ret_type);
};

// ExprAssembler }}

}
