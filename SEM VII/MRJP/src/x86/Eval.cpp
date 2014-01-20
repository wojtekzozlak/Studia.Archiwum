#include "Eval.h"
#include "Assembler.h"
#include "Operations.h"

namespace x86 {

namespace __op = Operations;


// {{{ OpEval

OpEval::OpEval(Core::AbstractType::Ptr t, Visitable* op,
    ExprAssembler* ass) : op(op), ass(ass) {
  t->accept(this);
};


void OpEval::visitInt(Core::IntType* t) {
  IntEval(op, ass);
};


void OpEval::visitVoid(Core::VoidType* t) {
  assert(false);
};


void OpEval::visitString(Core::StringType* t) {
  StringEval(op, ass);
};


void OpEval::visitInvalid(Core::InvalidType* t) {
  assert(false);
};


void OpEval::visitBool(Core::BoolType* t) {
  BoolEval(op, ass);
};


void OpEval::visitFun(Core::FunType* t) {
  assert(false);
};


void OpEval::visitArray(Core::ArrayType* t) {
  ArrayEval(op, ass);
};


void OpEval::visitClass(Core::ClassType* t) {
  ClassEval(op, ass);
};


void OpEval::visitNull(Core::NullType* t) {
  assert(false);
};

// OpEval }}}


// {{{ IntEval

IntEval::IntEval(Visitable* op, ExprAssembler* ass) : ass(ass) {
  type_size = TypeSize::getSize(Core::IntType::obj().get());
  crystalize = ass->if_true.compare(std::string()) == 0;
  op->accept(this);
};


void IntEval::visitNoInit(NoInit* p) {
  __op::movregl(ass->ass_env->stream, 0, ass->dest);
};


void IntEval::visitEVar(EVar* p) {
  __op::movl(ass->ass_env->stream, ass->arg1, ass->dest);
};


void IntEval::visitPlus(Plus* p) {
  __op::movregl(ass->ass_env->stream, ass->arg1, "%ecx");
  __op::addregl(ass->ass_env->stream, ass->arg2, "%ecx");
  __op::movregl(ass->ass_env->stream, "%ecx", ass->dest);
};


void IntEval::visitMinus(Minus* p) {
  __op::movregl(ass->ass_env->stream, ass->arg1, "%ecx");
  __op::subregl(ass->ass_env->stream, ass->arg2, "%ecx");
  __op::movregl(ass->ass_env->stream, "%ecx", ass->dest);
};


void IntEval::visitTimes(Times* p) {
  __op::movregl(ass->ass_env->stream, ass->arg1, "%ecx");
  __op::imul(ass->ass_env->stream, ass->arg2, "%ecx");
  __op::movregl(ass->ass_env->stream, "%ecx", ass->dest);
};


void IntEval::visitNeg(Neg* p) {
  __op::movregl(ass->ass_env->stream, ass->arg1, "%ecx");
  __op::negl(ass->ass_env->stream, "%ecx");
  __op::movregl(ass->ass_env->stream, "%ecx", ass->dest);
};


void IntEval::visitDiv(Div* p) {
  __op::movregl(ass->ass_env->stream, ass->arg1, "%eax");
  __op::movregl(ass->ass_env->stream, "%eax", "%edx");
  __op::sarl(ass->ass_env->stream, 31, "%edx");
  __op::idivl(ass->ass_env->stream, ass->arg2);
  __op::movregl(ass->ass_env->stream, "%eax", ass->dest);
};


void IntEval::visitMod(Mod* p) {
  __op::movregl(ass->ass_env->stream, ass->arg1, "%eax");
  __op::movregl(ass->ass_env->stream, "%eax", "%edx");
  __op::sarl(ass->ass_env->stream, 31, "%edx");
  __op::idivl(ass->ass_env->stream, ass->arg2);
  __op::movregl(ass->ass_env->stream, "%edx", ass->dest);
};


void IntEval::visitEQU(EQU* p) {
  __op::rel("jz", ass, type_size);
};


void IntEval::visitNE(NE* p) {
  __op::rel("jnz", ass, type_size);
};


void IntEval::visitGE(GE* p) {
  __op::rel("jnl", ass, type_size);
};


void IntEval::visitLE(LE* p) {
  __op::rel("jng", ass, type_size);
};


void IntEval::visitGTH(GTH* p) {
  __op::rel("jnle", ass, type_size);
};

void IntEval::visitLTH(LTH* p) {
  __op::rel("jnge", ass, type_size);
};

// IntEval }}}


// {{{ BoolEval

BoolEval::BoolEval(Visitable* op, ExprAssembler* ass) : ass(ass) {
  type_size = TypeSize::getSize(Core::BoolType::obj().get());
  crystalize = ass->if_true.compare(std::string()) == 0;
  op->accept(this);
};


void BoolEval::visitNoInit(NoInit* p) {
  __op::movb(ass->ass_env->stream, 0, ass->dest);
};


void BoolEval::visitEVar(EVar* p) {
  if (crystalize)
    __op::movb(ass->ass_env->stream, ass->arg1, ass->dest);
  else {
    __op::cmpb(ass->ass_env->stream, ass->arg1, "$1");
    __op::cndjmp(ass->ass_env->stream, "jz", ass->if_true);
    __op::jmp(ass->ass_env->stream, ass->if_false);
  }
};


void BoolEval::visitEQU(EQU* p) {
  __op::rel("jz", ass, type_size);
};


void BoolEval::visitNE(NE* p) {
  __op::rel("jnz", ass, type_size);
};

// BoolEval }}}


// {{{ StringEval

StringEval::StringEval(Visitable* op, ExprAssembler* ass) : ass(ass) {
  type_size = TypeSize::getSize(Core::StringType::obj().get());
  op->accept(this);
};


void StringEval::visitNoInit(NoInit* p) {
  __op::movlabl(ass->ass_env->stream, ".emptyVal", ass->dest);
};


void StringEval::visitEVar(EVar* p) {
  __op::movl(ass->ass_env->stream, ass->arg1, ass->dest);
};


void StringEval::visitEQU(EQU* p) {
  __op::rel("jz", ass, type_size);
};


void StringEval::visitNE(NE* p) {
  __op::rel("jnz", ass, type_size);
};


void StringEval::visitPlus(Plus* p) {
  __op::movsp(ass->ass_env->stream, (ass->sp - 8) & -16);
  __op::movl(ass->ass_env->stream, ass->arg1, "(%esp)");
  __op::movl(ass->ass_env->stream, ass->arg2, "4(%esp)");
  __op::call(ass->ass_env->stream, ".concat");
  __op::movregl(ass->ass_env->stream, "%eax", ass->dest);
};

// StringEval }}}


// {{{ ArrayEval

ArrayEval::ArrayEval(Visitable* op, ExprAssembler* ass) : ass(ass) {
  op->accept(this);
};


void ArrayEval::visitNoInit(NoInit* p) {
  __op::movlabl(ass->ass_env->stream, ".emptyVal", ass->dest);
};



void ArrayEval::visitEVar(EVar* p) {
  __op::movl(ass->ass_env->stream, ass->arg1, ass->dest);
};


void ArrayEval::visitEGetArray(EGetArray* p) {
  std::ostream& out = ass->ass_env->stream;

  Core::ArrayType* arr = (Core::ArrayType*) p->expr_1->type.get();
  int type_size = TypeSize::getSize(arr->elementsType.get());

  __op::movregl(out, ass->arg1, "%ecx");
  __op::movregl(out, ass->arg2, "%edx");

  if (ass->lvalue) {
    if (type_size == 4)
      __op::sal(out, 2, "%edx");
    __op::addregl(out, "%edx", "%ecx");
    __op::movregl(out, "%ecx", ass->dest);
  } else {
    if (type_size == 4)
      __op::movl(out, "(%ecx, %edx, 4)", ass->dest);
    else
      __op::movb(out, "(%ecx, %edx, 1)", ass->dest);
  }
};


void ArrayEval::visitEAttr(EAttr* p) {
  // Return length.
  __op::movl(ass->ass_env->stream, ass->arg1, "%ecx");
  __op::movl(ass->ass_env->stream, "-4(%ecx)", "%edx");
  __op::movl(ass->ass_env->stream, "%edx", ass->dest);
};

// ArrayEval }}}


// {{{ ClassEval

ClassEval::ClassEval(Visitable* op, ExprAssembler* ass) : ass(ass) {
  type_size = TypeSize::getSize(Core::StringType::obj().get());
  op->accept(this);
};


void ClassEval::visitNoInit(NoInit* p) {
  __op::movl(ass->ass_env->stream, 0, ass->dest);
};


void ClassEval::visitEVar(EVar* p) {
  __op::movl(ass->ass_env->stream, ass->arg1, ass->dest);
};


void ClassEval::visitEAttr(EAttr* p) {
  std::ostream& out = ass->ass_env->stream;

  int offset = ((Core::ClassType*) p->expr_->type.get())->attrOffset(p->ident_);
  int type_size = TypeSize::getSize(p->type.get());

  __op::movregl(out, ass->arg1, "%ecx");
  __op::movl(out, offset, "%edx");

  if (ass->lvalue) {
    __op::addregl(out, "%edx", "%ecx");
    __op::movregl(out, "%ecx", ass->dest);
  } else {
    if (type_size == 4)
      __op::movl(out, "(%ecx, %edx, 1)", ass->dest);
    else
      __op::movb(out, "(%ecx, %edx, 1)", ass->dest);
  }
};


void ClassEval::visitEQU(EQU* p) {
  __op::rel("jz", ass, type_size);
};


void ClassEval::visitNE(NE* p) {
  __op::rel("jnz", ass, type_size);
};


// ClassEval }}}

}
