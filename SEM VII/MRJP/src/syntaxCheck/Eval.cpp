#include "Eval.h"

#define INVALID_TYPE_D(m) (InvalidTypePtr(new InvalidType(m)))
#define INVALID_TYPE_D1(m, a1) (InvalidTypePtr(new InvalidType(m, a1)))
#define INVALID_TYPE_D2(m, a1, a2) (InvalidTypePtr(new InvalidType(m, a1, a2)))

namespace SyntaxCheck {

typedef boost::shared_ptr<IntType> IntTypePtr;
typedef boost::shared_ptr<BoolType> BoolTypePtr;
typedef boost::shared_ptr<InvalidType> InvalidTypePtr;


// {{{ UnaryOpEval

UnaryOpEval::UnaryOpEval(AbstractType::Ptr t, Visitable* op) {
  this->op = op;
  t->accept(this);
  assert(result.get() != NULL);
};


void UnaryOpEval::visitInt(IntType* t) {
  result = IntEval(t, op).ret;
};


void UnaryOpEval::visitVoid(VoidType* t) {
  result = InvalidType::obj();
};


void UnaryOpEval::visitString(StringType* t) {
  result = StringEval(t, op).ret;
};


void UnaryOpEval::visitInvalid(InvalidType* t) {
  result = InvalidType::obj();
};


void UnaryOpEval::visitBool(BoolType* t) {
  result = BoolEval(t, op).ret;
};


void UnaryOpEval::visitFun(FunType* t) {
  result = InvalidType::obj();
};


void UnaryOpEval::visitArray(ArrayType* t) {
  result = InvalidType::obj();
};


void UnaryOpEval::visitClass(ClassType* t) {
  result = ClassEval(t, op).ret;
};


void UnaryOpEval::visitNull(NullType* t) {
  result = InvalidType::obj();
};

// UnaryOpEval }}}



// {{{ BinaryOpEval

BinaryOpEval::BinaryOpEval(AbstractType::Ptr t, AbstractType::Ptr t2,
    Visitable* op) {
  this->op = op;
  this->t2 = t2;
  t->accept(this);
  assert(result.get() != NULL);
};


void BinaryOpEval::visitInt(IntType* t) {
  result = IntEval(t, t2.get(), op).ret;
};


void BinaryOpEval::visitVoid(VoidType* t) {
  result = InvalidType::obj();
};


void BinaryOpEval::visitString(StringType* t) {
  result = StringEval(t, t2.get(), op).ret;
};


void BinaryOpEval::visitInvalid(InvalidType* t) {
  result = InvalidType::obj();
};


void BinaryOpEval::visitBool(BoolType* t) {
  result = BoolEval(t, t2.get(), op).ret;
};


void BinaryOpEval::visitFun(FunType* t) {
  result = InvalidType::obj();
};


void BinaryOpEval::visitArray(ArrayType* t) {
  result = InvalidType::obj();
};


void BinaryOpEval::visitClass(ClassType* t) {
  result = ClassEval(t, t2.get(), op).ret;
};


void BinaryOpEval::visitNull(NullType* t) {
  result = InvalidType::obj();
};

// BinaryOpEval }}}


// {{{ AbstractEval

void AbstractEval::visitNot(Not* p) {
  ret = InvalidType::withStdReason(p, a1);
}

void AbstractEval::visitNeg(Neg* p) {
  ret = InvalidType::withStdReason(p, a1);
}

void AbstractEval::visitTimes(Times* p) {
  ret = InvalidType::withStdReason(p, a1, a2);
}

void AbstractEval::visitDiv(Div* p) {
  ret = InvalidType::withStdReason(p, a1, a2);
}

void AbstractEval::visitMod(Mod* p) {
  ret = InvalidType::withStdReason(p, a1, a2);
}

void AbstractEval::visitPlus(Plus* p) {
  ret = InvalidType::withStdReason(p, a1, a2);
}

void AbstractEval::visitMinus(Minus* p) {
  ret = InvalidType::withStdReason(p, a1, a2);
}

void AbstractEval::visitLTH(LTH* p) {
  ret = InvalidType::withStdReason(p, a1, a2);
}

void AbstractEval::visitLE(LE* p) {
  ret = InvalidType::withStdReason(p, a1, a2);
}

void AbstractEval::visitGTH(GTH* p) {
  ret = InvalidType::withStdReason(p, a1, a2);
}

void AbstractEval::visitGE(GE* p) {
  ret = InvalidType::withStdReason(p, a1, a2);
}

void AbstractEval::visitEQU(EQU* p) {
  ret = InvalidType::withStdReason(p, a1, a2);
}

void AbstractEval::visitNE(NE* p) {
  ret = InvalidType::withStdReason(p, a1, a2);
}

void AbstractEval::visitEOr(EOr* p) {
  ret = InvalidType::withStdReason(p, a1, a2);
}

void AbstractEval::visitEAnd(EAnd* p) {
  ret = InvalidType::withStdReason(p, a1, a2);
}

// AbstractEval }}}


// {{{ StringEval

StringEval::StringEval(AbstractType* s1) : s2(NULL) {
  this->a1 = s1;
  this->s1 = dynamic_cast<StringType*>(s1);
};


StringEval::StringEval(AbstractType* s1, Visitable* op) : s2(NULL) {
  this->a1 = s1;
  this->s1 = dynamic_cast<StringType*>(s1);
  op->accept(this);
  assert(ret.get() != NULL);
};


StringEval::StringEval(AbstractType *s1, AbstractType *s2) {
  this->a1 = s1;
  this->a2 = s2;
  this->s1 = dynamic_cast<StringType*>(s1);
  this->s2 = dynamic_cast<StringType*>(s2);
};


StringEval::StringEval(AbstractType *s1, AbstractType *s2, Visitable* op) {
  this->a1 = s1;
  this->a2 = s2;
  this->s1 = dynamic_cast<StringType*>(s1);
  this->s2 = dynamic_cast<StringType*>(s2);
  op->accept(this);
  assert(ret.get() != NULL);
};


bool StringEval::trivialCase(Visitable *op) {
  if (s1 == NULL || s2 == NULL) ret = InvalidType::withStdReason(op, a1, a2);
  else return false;

  return true;
}


void StringEval::visitPlus(Plus* p) {
  if (!trivialCase(p))
    ret = StringType::obj()->toRValue();
}


void StringEval::visitEQU(EQU* p) {
  if (!trivialCase(p))
    ret = BoolType::obj()->toRValue();
};


void StringEval::visitNE(NE* p) {
  if (!trivialCase(p))
    ret = BoolType::obj()->toRValue();
};

// StringEval }}}


// {{{ ClassEval

ClassEval::ClassEval(AbstractType* c1) : c2(NULL) {
  this->a1 = c1;
  this->c1 = dynamic_cast<ClassType*>(c1);
};


ClassEval::ClassEval(AbstractType* c1, Visitable* op) : c2(NULL) {
  this->a1 = c1;
  this->c1 = dynamic_cast<ClassType*>(c1);
  op->accept(this);
  assert(ret.get() != NULL);
};


ClassEval::ClassEval(AbstractType* c1, AbstractType* c2) {
  this->a1 = c1;
  this->a2 = c2;
  this->c1 = dynamic_cast<ClassType*>(c1);
  this->c2 = dynamic_cast<ClassType*>(c2);
};


ClassEval::ClassEval(AbstractType* c1, AbstractType* c2, Visitable* op) {
  this->a1 = c1;
  this->a2 = c2;
  this->c1 = dynamic_cast<ClassType*>(c1);
  this->c2 = dynamic_cast<ClassType*>(c2);
  op->accept(this);
  assert(ret.get() != NULL);
};


bool ClassEval::cmpCase(Visitable *op) {
  if (c1 == NULL || c2 == NULL) ret = InvalidType::withStdReason(op, a1, a2);
  else if (c1->name.compare(c2->name) == 0) ret = BoolType::obj()->toRValue();
  else ret = InvalidType::withStdReason(op, a1, a2);
};


void ClassEval::visitEQU(EQU* p) {
  cmpCase(p);
};


void ClassEval::visitNE(NE* p) {
  cmpCase(p);
};

// ClassEval }}}


// {{{ IntEval

IntEval::IntEval(AbstractType *i1) : i2(NULL) {
  this->a1 = i1;
  this->i1 = dynamic_cast<IntType*>(i1);
};


IntEval::IntEval(AbstractType *i1, Visitable *op) : i2(NULL) {
  this->a1 = i1;
  this->i1 = dynamic_cast<IntType*>(i1);
  op->accept(this);
  assert(ret.get() != NULL);
};


IntEval::IntEval(AbstractType *i1, AbstractType *i2) {
  this->a1 = i1;
  this->a2 = i2;
  this->i1 = dynamic_cast<IntType*>(i1);
  this->i2 = dynamic_cast<IntType*>(i2);
};


IntEval::IntEval(AbstractType *i1, AbstractType *i2, Visitable* op) {
  this->a1 = i1;
  this->a2 = i2;
  this->i1 = dynamic_cast<IntType*>(i1);
  this->i2 = dynamic_cast<IntType*>(i2);
  op->accept(this);
  assert(ret.get() != NULL);
};


bool IntEval::trivialCase(Visitable *op, AbsPtrType type) {
  if(i1 == NULL || i2 == NULL) ret = InvalidType::withStdReason(op, a1, a2);
  else if (i1->unknown || i2->unknown) ret = type->toRValue();
  else return false;

  return true;
};


void IntEval::visitNeg(Neg* p) {
  if (i1 == NULL) ret = InvalidType::withStdReason(p, a1);
  else if(i1->unknown) ret = IntTypePtr(i1);
  else ret = IntType(-i1->value).toRValue();
}


void IntEval::visitTimes(Times* p) {
  if (!trivialCase(p, IntType::obj()))
    ret = IntType(i1->value * i2->value).toRValue();
};


void IntEval::visitDiv(Div* p) {
  if (!trivialCase(p, IntType::obj())) {
    if (i2->value == 0) ret = INVALID_TYPE_D("zero division");
    else ret = IntType(i1->value / i2->value).toRValue();
  }
};


void IntEval::visitMod(Mod* p) {
  if (!trivialCase(p, IntType::obj())) {
    if (i2->value == 0) ret = INVALID_TYPE_D("zero modulo");
    else ret = IntType(i1->value % i2->value).toRValue();
  }
};


void IntEval::visitPlus(Plus* p) {
  if (!trivialCase(p, IntType::obj()))
    ret = IntType(i1->value + i2->value).toRValue();
};


void IntEval::visitMinus(Minus* p) {
  if (!trivialCase(p, IntType::obj()))
    ret = IntType(i1->value - i2->value).toRValue();
};


void IntEval::visitLTH(LTH* p) {
  if (!trivialCase(p, BoolType::obj()))
    ret = BoolType(i1->value < i2->value).toRValue();
};


void IntEval::visitLE(LE* p) {
  if (!trivialCase(p, BoolType::obj()))
    ret = BoolType(i1->value <= i2->value).toRValue();
};


void IntEval::visitGTH(GTH* p) {
  if (!trivialCase(p, BoolType::obj()))
    ret = BoolType(i1->value > i2->value).toRValue();
};


void IntEval::visitGE(GE* p) {
  if (!trivialCase(p, BoolType::obj()))
    ret = BoolType(i1->value > i2->value).toRValue();
};


void IntEval::visitEQU(EQU* p) {
  if (!trivialCase(p, BoolType::obj()))
    ret = BoolType(i1->value == i2->value).toRValue();
};


void IntEval::visitNE(NE* p) {
  if (!trivialCase(p, BoolType::obj()))
    ret = BoolType(i1->value != i2->value).toRValue();
};

// IntVal }}}


// {{{ BoolEval

BoolEval::BoolEval(AbstractType *b1) : b2(NULL) {
  this->a1 = b1;
  this->b1 = dynamic_cast<BoolType*>(b1);
};


BoolEval::BoolEval(AbstractType* b1, Visitable* op) : b2(NULL) {
  this->a1 = b1;
  this->b1 = dynamic_cast<BoolType*>(b1);
  op->accept(this);
  assert(ret.get() != NULL);
};


BoolEval::BoolEval(AbstractType *b1, AbstractType *b2) {
  this->a1 = b1;
  this->a2 = b2;
  this->b1 = dynamic_cast<BoolType*>(b1);
  this->b2 = dynamic_cast<BoolType*>(b2);
};


BoolEval::BoolEval(AbstractType* b1, AbstractType* b2, Visitable* op) {
  this->a1 = b1;
  this->a2 = b2;
  this->b1 = dynamic_cast<BoolType*>(b1);
  this->b2 = dynamic_cast<BoolType*>(b2);
  op->accept(this);
  assert(ret.get() != NULL);
};


bool BoolEval::trivialCase() {
  if(b1 == NULL || b2 == NULL) ret = INVALID_TYPE;
  else if (b1->unknown || b2->unknown) ret = BoolType().toRValue();
  else return false;

  return true;
}


void BoolEval::visitNot(Not* p) {
  if (b1 == NULL) ret = INVALID_TYPE;
  else if(b1->unknown) ret = BoolType::obj()->toRValue();
  else ret = BoolType(!b1->value).toRValue();
}


void BoolEval::visitEQU(EQU* p) {
  if (!trivialCase())
    ret = BoolType(b1->value == b2->value).toRValue();
}


void BoolEval::visitNE(NE* p) {
  if (!trivialCase())
    ret = BoolType(b1->value != b2->value).toRValue();
}


void BoolEval::visitEAnd(EAnd* p) {
  if (!trivialCase())
    ret = BoolType(b1->value && b2->value).toRValue();
}


void BoolEval::visitEOr(EOr* p) {
  if (!trivialCase())
    ret = BoolType(b1->value || b2->value).toRValue();
}

// BoolEval }}}


}
