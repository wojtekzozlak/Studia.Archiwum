#ifndef _X86_EVAL_
#define _X86_EVAL_

#include "core/Types.h"
#include "parser/VisitorSkeleton.h"

namespace x86 {

class ExprAssembler;


class OpEval : public Core::TypeVisitor {
  Visitable* op;
  ExprAssembler* ass;

  public:
    Core::AbstractType::Ptr result;
    OpEval(Core::AbstractType::Ptr t, Visitable *op, ExprAssembler* ass);

    void visitInt(Core::IntType* t);
    void visitVoid(Core::VoidType* t);
    void visitString(Core::StringType* t);
    void visitInvalid(Core::InvalidType* t);
    void visitBool(Core::BoolType* t);
    void visitFun(Core::FunType* t);
    void visitArray(Core::ArrayType* t);
    void visitClass(Core::ClassType* t);
    void visitNull(Core::NullType* t);
};


/* Ewaluator dla liczb całkowitych. */
class IntEval : public VisitorSkeleton {
  private:
    ExprAssembler* ass;
    int type_size;
    bool crystalize;

  public:
    IntEval(Visitable* op, ExprAssembler* ass);

    void visitNoInit(NoInit* p);

    void visitEVar(EVar* p);

    void visitPlus(Plus* p);
    void visitMinus(Minus* p);

    void visitTimes(Times* p);

    void visitEQU(EQU* p);
    void visitNE(NE* p);
    void visitGE(GE* p);
    void visitLE(LE* p);
    void visitGTH(GTH* p);
    void visitLTH(LTH* p);

    void visitNeg(Neg* p);

    void visitMod(Mod* p);
    void visitDiv(Div* p);
};


/* Ewaluator dla wartości logicznych. */
class BoolEval : public VisitorSkeleton {
  private:
    ExprAssembler* ass;
    int type_size;
    int crystalize;

  public:
    BoolEval(Visitable* op, ExprAssembler* ass);

    void visitNoInit(NoInit* p);
    void visitEVar(EVar* p);
    void visitEQU(EQU* p);
    void visitNE(NE* p);
};


/* Ewaluator dla ciągów znaków. */
class StringEval : public VisitorSkeleton {
  private:
    ExprAssembler* ass;
    int type_size;

  public:
    StringEval(Visitable* op, ExprAssembler* ass);

    void visitNoInit(NoInit* p);
    void visitEVar(EVar* p);
    void visitPlus(Plus* p);
    void visitEQU(EQU* p);
    void visitNE(NE* p);
};


/* Ewaluator dla ciągów tablic. */
class ArrayEval : public VisitorSkeleton {
  private:
    ExprAssembler* ass;

  public:
    ArrayEval(Visitable* op, ExprAssembler* ass);

    void visitNoInit(NoInit* p);
    void visitEVar(EVar* p);
    void visitEGetArray(EGetArray* p);
    void visitEAttr(EAttr* p);
};


class ClassEval : public VisitorSkeleton {
  private:
    ExprAssembler* ass;
    int type_size;

  public:
    ClassEval(Visitable* op, ExprAssembler* ass);

    void visitNoInit(NoInit* p);
    void visitEVar(EVar* p);
    void visitEAttr(EAttr* p);

    void visitEQU(EQU* p);
    void visitNE(NE* p);
};


}

#endif
