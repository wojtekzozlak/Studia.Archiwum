/****************************************************************************
 * Eval.h
 *
 * Ewaluatory wyrażeń dla konkretnych typów.
 *
 * Biorą za argument dwa obiekty, zakładając typ pierwszego z nich i potrafią
 * zwrócić typ wyniku dla konkretnej operacji unarnej / binarnej. Korzystają
 * przy tym ze wzorca projektowego visitor i proszą węzeł operacji o wybranie
 * odpowiedniej implementacji.
 ****************************************************************************/
#ifndef _SYNTAXCHECKER_EVAL_
#define _SYNTAXCHECKER_EVAL_

#include <boost/shared_ptr.hpp>
#include "parser/VisitorSkeleton.h"
#include "core/Types.h"

namespace SyntaxCheck {

using namespace Core;

class UnaryOpEval : public TypeVisitor {
  Visitable *op;

  public:
    AbstractType::Ptr result;
    UnaryOpEval(AbstractType::Ptr t, Visitable *op);

    void visitInt(IntType* t);
    void visitVoid(VoidType* t);
    void visitString(StringType* t);
    void visitInvalid(InvalidType* t);
    void visitBool(BoolType* t);
    void visitFun(FunType* t);
    void visitArray(ArrayType* t);
    void visitClass(ClassType* t);
    void visitNull(NullType* t);
};


class BinaryOpEval : public TypeVisitor {
  AbstractType::Ptr t2;
  Visitable *op;

  public:
    AbstractType::PtrType result;
    BinaryOpEval(AbstractType::Ptr t, AbstractType::Ptr t2, Visitable* op);

    void visitInt(IntType* t);
    void visitVoid(VoidType* t);
    void visitString(StringType* t);
    void visitInvalid(InvalidType* t);
    void visitBool(BoolType* t);
    void visitFun(FunType* t);
    void visitArray(ArrayType* t);
    void visitClass(ClassType* t);
    void visitNull(NullType* t);
};


/* Domyślny ewaluator, który zawsze zwraca nieprawidłowy typ. */
class AbstractEval : public VisitorSkeleton {
  protected:
    AbstractType *a1;
    AbstractType *a2;

  public:
    AbstractEval() : a1(NULL), a2(NULL) {};

    boost::shared_ptr<AbstractType> ret;
    virtual void visitNot(Not* p);
    virtual void visitNeg(Neg* p);

    virtual void visitTimes(Times* p);
    virtual void visitDiv(Div* p);
    virtual void visitMod(Mod* p);

    virtual void visitPlus(Plus* p);
    virtual void visitMinus(Minus* p);

    virtual void visitLTH(LTH* p);
    virtual void visitLE(LE* p);
    virtual void visitGTH(GTH* p);
    virtual void visitGE(GE* p);
    virtual void visitEQU(EQU* p);
    virtual void visitNE(NE* p);

    virtual void visitEAnd(EAnd* p);
    virtual void visitEOr(EOr* p);
};


/* Ewaluator dla obiektów `null`. */
class NullEval : public AbstractEval {
  private:
    NullType* n1;
    NullType* n2;

    bool trivialCase(Visitable *op);

  public:
    NullEval(AbstractType* n1);
    NullEval(AbstractType* n1, AbstractType* n2);
};


/* Ewaluator dla ciągów znaków. */
class StringEval : public AbstractEval {
  private:
    StringType* s1;
    StringType* s2;

    bool trivialCase(Visitable *op);

  public:
    StringEval(AbstractType* s1);
    StringEval(AbstractType* s1, Visitable* op);
    StringEval(AbstractType* s1, AbstractType* s2);
    StringEval(AbstractType* s1, AbstractType* s2, Visitable* op);

    void visitPlus(Plus* p);
    void visitEQU(EQU* p);
    void visitNE(NE* p);
};


/* Ewaluator dla klas. */
class ClassEval : public AbstractEval {
  private:
    ClassType* c1;
    ClassType* c2;

    bool cmpCase(Visitable *op);

  public:
    ClassEval(AbstractType* c1);
    ClassEval(AbstractType* c1, Visitable* op);
    ClassEval(AbstractType* c1, AbstractType* c2);
    ClassEval(AbstractType* c1, AbstractType* c2, Visitable* op);

    void visitEQU(EQU* p);
    void visitNE(NE* p);
};


/* Ewaluator dla liczb całkowitych. */
class IntEval : public AbstractEval {
  private:
    IntType* i1;
    IntType* i2;

    bool trivialCase(Visitable *op, AbsPtrType type);

  public:
    IntEval(AbstractType* i1);
    IntEval(AbstractType* i1, Visitable* op);
    IntEval(AbstractType* i1, AbstractType* i2);
    IntEval(AbstractType* i1, AbstractType* i2, Visitable* op);

    void visitNeg(Neg* p);

    void visitTimes(Times* p);
    void visitDiv(Div* p);
    void visitMod(Mod* p);

    void visitPlus(Plus* p);
    void visitMinus(Minus* p);

    void visitLTH(LTH* p);
    void visitLE(LE* p);
    void visitGTH(GTH* p);
    void visitGE(GE* p);
    void visitEQU(EQU* p);
    void visitNE(NE* p);
};


/* Ewaluator dla wartości logicznych. */
class BoolEval : public AbstractEval {
  private:
    BoolType* b1;
    BoolType* b2;

    bool trivialCase();

  public:
    BoolEval(AbstractType* b1);
    BoolEval(AbstractType* b1, Visitable* op);
    BoolEval(AbstractType* b1, AbstractType* b2);
    BoolEval(AbstractType* b1, AbstractType* b2, Visitable* op);

    void visitNot(Not* p);
    void visitEQU(EQU* p);
    void visitNE(NE* p);

    void visitEAnd(EAnd* p);
    void visitEOr(EOr* p);
};

}

#endif
