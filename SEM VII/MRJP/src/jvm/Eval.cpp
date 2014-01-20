#include "Eval.h"
#include "Assembler.h"
#include <sstream>

namespace JVM {

// {{{ UnaryOpEval

UnaryOpEval::UnaryOpEval(Core::AbstractType::Ptr t, Visitable* op,
    ExprAssembler* ass) : op(op), ass(ass) {
  t->accept(this);
  assert(result.get() != NULL);
};


void UnaryOpEval::visitInt(Core::IntType* t) {
  result = IntEval(t, op, ass).ret;
};


void UnaryOpEval::visitVoid(Core::VoidType* t) {
  assert(false);
  result = Core::InvalidType::obj();
};


void UnaryOpEval::visitString(Core::StringType* t) {
  result = StringEval(t, op, ass).ret;
};


void UnaryOpEval::visitInvalid(Core::InvalidType* t) {
  assert(false);
  result = Core::InvalidType::obj();
};


void UnaryOpEval::visitBool(Core::BoolType* t) {
  result = BoolEval(t, op, ass).ret;
};


void UnaryOpEval::visitFun(Core::FunType* t) {
  assert(false);
  result = Core::InvalidType::obj();
};


void UnaryOpEval::visitArray(Core::ArrayType* t) {
  assert(false);
  result = Core::InvalidType::obj();
};


void UnaryOpEval::visitClass(Core::ClassType* t) {
  result = ClassEval(t, op).ret;
};


void UnaryOpEval::visitNull(Core::NullType* t) {
  assert(false);
  result = Core::InvalidType::obj();
};

// UnaryOpEval }}}


// {{{ BinaryOpEval

BinaryOpEval::BinaryOpEval(Core::AbstractType::Ptr t,
    Core::AbstractType::Ptr t2, Visitable* op, ExprAssembler* ass) : op(op),
    t2(t2), ass(ass) {
  t->accept(this);
  assert(result.get() != NULL);
};


void BinaryOpEval::visitInt(Core::IntType* t) {
  result = IntEval(t, t2.get(), op, ass).ret;
};


void BinaryOpEval::visitVoid(Core::VoidType* t) {
  result = Core::InvalidType::obj();
};


void BinaryOpEval::visitString(Core::StringType* t) {
  result = StringEval(t, t2.get(), op, ass).ret;
};


void BinaryOpEval::visitInvalid(Core::InvalidType* t) {
  result = Core::InvalidType::obj();
};


void BinaryOpEval::visitBool(Core::BoolType* t) {
  result = BoolEval(t, t2.get(), op, ass).ret;
};


void BinaryOpEval::visitFun(Core::FunType* t) {
  result = Core::InvalidType::obj();
};


void BinaryOpEval::visitArray(Core::ArrayType* t) {
  result = Core::InvalidType::obj();
};


void BinaryOpEval::visitClass(Core::ClassType* t) {
  result = ClassEval(t, t2.get(), op).ret;
};


void BinaryOpEval::visitNull(Core::NullType* t) {
  result = Core::InvalidType::obj();
};

// BinaryOpEval }}}

std::string make_label(const char* op, const char* type, int ix) {
  std::ostringstream ss;
  ss << op << "_" << type << "_" << ix;
  return ss.str();
};


Core::AbstractType::Ptr assemblyRel(std::ostream& stream,
    Core::BlockScope::PtrType scope, const char* relop, const char* jop,
    std::pair<std::string, std::string>* labels) {
  assert(labels != NULL);

  stream << "  " << jop << " " << labels->first << std::endl;
  stream << "  goto " << labels->second << std::endl;

  return Core::BoolType::obj();
};


// {{{ StringEval

StringEval::StringEval(Core::AbstractType* s1, Visitable* op,
    ExprAssembler* ass) : s2(NULL), ass(ass) {
  this->s1 = dynamic_cast<Core::StringType*>(s1);
  assert(this->s1 != NULL);
  op->accept(this);
  assert(ret.get() != NULL);
};


StringEval::StringEval(Core::AbstractType* s1, Core::AbstractType* s2,
    Visitable* op, ExprAssembler* ass) : ass(ass) {
  this->s1 = dynamic_cast<Core::StringType*>(s1);
  this->s2 = dynamic_cast<Core::StringType*>(s2);
  assert(this->s1 != NULL);
  assert(this->s2 != NULL);
  op->accept(this);
  assert(ret.get() != NULL);
};


void StringEval::visitPlus(Plus* p) {
  ass->stream << "  invokevirtual java/lang/String/concat(Ljava/lang/String;)Ljava/lang/String;"
      << std::endl;

  ret = Core::StringType::obj();
};


void StringEval::visitEQU(EQU* p) {
  ret = assemblyRel(ass->stream, ass->scope, "AEQU", "if_acmpeq",
      ass->nextLabelPair());
};


void StringEval::visitNE(NE* p) {
  ret = assemblyRel(ass->stream, ass->scope, "ANE", "if_acmpne",
      ass->nextLabelPair());
};


void StringEval::visitNoInit(NoInit* p) {
  ass->scope->define(p->ident_, Core::StringType::obj());
  ass->stream << "  ldc \"\"" << std::endl;
  ass->stream << "  astore " << ass->scope->get(p->ident_).first << std::endl;
  ret = Core::StringType::obj();
};


void StringEval::visitInit(Init* p) {
  ass->scope->define(p->ident_, Core::StringType::obj());
  ass->stream << "  astore " << ass->scope->get(p->ident_).first << std::endl;
  ret = Core::StringType::obj();
};


void StringEval::visitEVar(EVar* p) {
  ass->stream << "  aload " << ass->scope->get(p->ident_).first << std::endl;
  ret = Core::StringType::obj();
};

void StringEval::visitRet(Ret* p) {
  ass->stream << "  areturn" << std::endl;
  ret = Core::StringType::obj();
};

// StringEval }}}


// {{{ IntEval

IntEval::IntEval(Core::AbstractType* i1, Visitable* op, ExprAssembler* ass)
     : i2(NULL), ass(ass) {
  this->i1 = dynamic_cast<Core::IntType*>(i1);
  assert(this->i1 != NULL);
  op->accept(this);
  assert(ret.get() != NULL);
};


IntEval::IntEval(Core::AbstractType* i1, Core::AbstractType* i2, Visitable* op,
    ExprAssembler* ass) : ass(ass) {
  this->i1 = dynamic_cast<Core::IntType*>(i1);
  this->i2 = dynamic_cast<Core::IntType*>(i2);
  assert(this->i1 != NULL);
  assert(this->i2 != NULL);
  op->accept(this);
  assert(ret.get() != NULL);
};


void IntEval::visitNoInit(NoInit* p) {
  ass->scope->define(p->ident_, Core::IntType::obj());
  ass->stream << "  iconst_0" << std::endl;
  ass->stream << "  istore " << ass->scope->get(p->ident_).first << std::endl;
  ret = Core::IntType::obj();
};


void IntEval::visitInit(Init* p) {
  ass->scope->define(p->ident_, Core::IntType::obj());
  ass->stream << "  istore " << ass->scope->get(p->ident_).first << std::endl;
  ret = Core::IntType::obj();
};


void IntEval::visitEVar(EVar* p) {
  ass->stream << "  iload " << ass->scope->get(p->ident_).first << std::endl;
  ret = Core::IntType::obj();
};

void IntEval::visitRet(Ret* p) {
  ass->stream << "  ireturn" << std::endl;
  ret = Core::IntType::obj();
};


void IntEval::visitNeg(Neg* p) {
  ass->stream << "  ineg" << std::endl;
  ret = Core::IntType::obj();
};


void IntEval::visitTimes(Times* p) {
  ass->stream << "  imul" << std::endl;
  ret = Core::IntType::obj();
};


void IntEval::visitDiv(Div* p) {
  ass->stream << "  idiv" << std::endl;
  ret = Core::IntType::obj();
};


void IntEval::visitMod(Mod* p) {
  ass->stream << "  irem" << std::endl;
  ret = Core::IntType::obj();
};


void IntEval::visitPlus(Plus* p) {
  ass->stream << "  iadd" << std::endl;
  ret = Core::IntType::obj();
};


void IntEval::visitMinus(Minus* p) {
  ass->stream << "  isub" << std::endl;
  ret = Core::IntType::obj();
};


void IntEval::visitLTH(LTH* p) {
  ret = assemblyRel(ass->stream, ass->scope, "ILTH", "if_icmplt",
      ass->nextLabelPair());
};


void IntEval::visitLE(LE* p) {
  ret = assemblyRel(ass->stream, ass->scope, "ILE", "if_icmple",
      ass->nextLabelPair());
};


void IntEval::visitGTH(GTH* p) {
  ret = assemblyRel(ass->stream, ass->scope, "IGTH", "if_icmpgt",
      ass->nextLabelPair());
};


void IntEval::visitGE(GE* p) {
  ret = assemblyRel(ass->stream, ass->scope, "IGE", "if_icmpge",
      ass->nextLabelPair());
};


void IntEval::visitEQU(EQU* p) {
  ret = assemblyRel(ass->stream, ass->scope, "IEQU", "if_icmpeq",
      ass->nextLabelPair());
};


void IntEval::visitNE(NE* p) {
  ret = assemblyRel(ass->stream, ass->scope, "INE", "if_icmpne",
      ass->nextLabelPair());
};

// IntEval }}}


// {{{ ClassEval

ClassEval::ClassEval(Core::AbstractType* c1, Visitable* op) : c2(NULL) {
  this->c1 = dynamic_cast<Core::ClassType*>(c1);
  assert(this->c1 != NULL);
  op->accept(this);
  assert(ret.get() != NULL);
};


ClassEval::ClassEval(Core::AbstractType* c1, Core::AbstractType* c2,
    Visitable* op) {
  this->c1 = dynamic_cast<Core::ClassType*>(c1);
  this->c2 = dynamic_cast<Core::ClassType*>(c2);
  assert(this->c1 != NULL);
  assert(this->c2 != NULL);
  op->accept(this);
  assert(ret.get() != NULL);
};


void ClassEval::visitEQU(EQU* p) {
};


void ClassEval::visitNE(NE* p) {

};

// ClassEval }}}


// {{{ BoolEval

BoolEval::BoolEval(Core::AbstractType* b1, Visitable* op, ExprAssembler* ass)
    : b2(NULL), ass(ass) {
  this->b1 = dynamic_cast<Core::BoolType*>(b1);
  assert(this->b1 != NULL);
  op->accept(this);
  assert(ret.get() != NULL);
};


BoolEval::BoolEval(Core::AbstractType* b1, Core::AbstractType* b2,
    Visitable* op, ExprAssembler* ass) : ass(ass) {
  this->b1 = dynamic_cast<Core::BoolType*>(b1);
  this->b2 = dynamic_cast<Core::BoolType*>(b2);
  assert(this->b1 != NULL);
  assert(this->b2 != NULL);
  op->accept(this);
  assert(ret.get() != NULL);
};


void BoolEval::visitRet(Ret* p) {
  ass->stream << "  ireturn" << std::endl;
  ret = Core::BoolType::obj();
};


void BoolEval::visitNot(Not* p) {
  ret = Core::BoolType::obj();
};


void BoolEval::visitEQU(EQU* p) {
  ret = assemblyRel(ass->stream, ass->scope, "IEQU", "if_icmpeq",
      ass->nextLabelPair());
  ret = Core::BoolType::obj();
};


void BoolEval::visitNE(NE* p) {
  ret = assemblyRel(ass->stream, ass->scope, "IEQU", "if_icmpne",
      ass->nextLabelPair());
  ret = Core::BoolType::obj();
};


void BoolEval::visitEAnd(EAnd* p) {
  ret = Core::BoolType::obj();
};


void BoolEval::visitEOr(EOr* p) {
  ret = Core::BoolType::obj();
};


void BoolEval::visitNoInit(NoInit* p) {
  ass->scope->define(p->ident_, Core::BoolType::obj());
  ass->stream << "  iconst_0" << std::endl;
  ass->stream << "  istore " << ass->scope->get(p->ident_).first << std::endl;
  ret = Core::BoolType::obj();
};


void BoolEval::visitInit(Init* p) {
  ass->scope->define(p->ident_, Core::BoolType::obj());
  ass->stream << "  istore " << ass->scope->get(p->ident_).first << std::endl;
  ret = Core::BoolType::obj();
};


void BoolEval::visitEVar(EVar* p) {
  LabelPair* labels = ass->nextLabelPair();
  ass->stream << "  iload " << ass->scope->get(p->ident_).first << std::endl;
  ass->stream << "  ifeq " << labels->second << std::endl;
  ass->stream << "  goto " << labels->first << std::endl;
  ret = Core::BoolType::obj();
};

// BoolEval }}}


}
