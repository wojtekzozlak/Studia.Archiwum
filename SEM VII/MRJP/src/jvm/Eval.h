/****************************************************************************
 * Eval.h
 *
 * Ewaluatory wyrażeń przy kompilacji JVM.
 *
 * Bardzo podobnie jak przy type-checkingu, z dodatkiem możliwości pisania po
 * strumieniu wyjściowym i pobierania etykiet do ewentualnych skoków.
 ****************************************************************************/
#ifndef _JVM_EVAL_
#define _JVM_EVAL_

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <utility>
#include "parser/VisitorSkeleton.h"
#include "core/Environment.h"
#include "core/Types.h"

namespace JVM {

class ExprAssembler;
typedef std::pair<std::string, std::string> LabelPair;

/* Klasa delegująca wyrażenia unarne. */
class UnaryOpEval : public Core::TypeVisitor {
  Visitable* op;
  ExprAssembler* ass;

  public:
    Core::AbstractType::Ptr result;
    UnaryOpEval(Core::AbstractType::Ptr t, Visitable *op, ExprAssembler* ass);

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


/* Klasa delegująca wyrażenia binarne. */
class BinaryOpEval : public Core::TypeVisitor {
  Core::AbstractType::Ptr t2;
  Visitable* op;
  ExprAssembler* ass;

  public:
    Core::AbstractType::Ptr result;
    BinaryOpEval(Core::AbstractType::Ptr t, Core::AbstractType::Ptr t2,
        Visitable* op, ExprAssembler* ass);

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


/* Ewaluator dla ciągów znaków. */
class StringEval : public VisitorSkeleton {
  private:
    Core::StringType* s1;
    Core::StringType* s2;
    ExprAssembler* ass;

  public:
    Core::AbstractType::PtrType ret;

    StringEval(Core::AbstractType* s1, Visitable* op, ExprAssembler* ass);
    StringEval(Core::AbstractType* s1, Core::AbstractType* s2, Visitable* op,
        ExprAssembler* ass);

    void visitRet(Ret* p);
    void visitNoInit(NoInit* p);
    void visitInit(Init* p);
    void visitEVar(EVar* p);

    void visitPlus(Plus* p);
    void visitEQU(EQU* p);
    void visitNE(NE* p);
};


/* Ewaluator dla liczb całkowitych. */
class IntEval : public VisitorSkeleton {
  private:
    Core::IntType* i1;
    Core::IntType* i2;
    ExprAssembler* ass;

  public:
    Core::AbstractType::PtrType ret;

    IntEval(Core::AbstractType* i1, Visitable* op, ExprAssembler* ass);
    IntEval(Core::AbstractType* i1, Core::AbstractType* i2, Visitable* op,
        ExprAssembler* ass);

    void visitRet(Ret* p);
    void visitNoInit(NoInit* p);
    void visitInit(Init* p);
    void visitEVar(EVar* p);

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


/* Ewaluator dla klas. */
class ClassEval : public VisitorSkeleton {
  private:
    Core::ClassType* c1;
    Core::ClassType* c2;

  public:
    Core::AbstractType::PtrType ret;

    ClassEval(Core::AbstractType* c1, Visitable* op);
    ClassEval(Core::AbstractType* c1, Core::AbstractType* c2, Visitable* op);

    void visitEQU(EQU* p);
    void visitNE(NE* p);
};


/* Ewaluator dla wartości logicznych. */
class BoolEval : public VisitorSkeleton {
  private:
    Core::BoolType* b1;
    Core::BoolType* b2;
    ExprAssembler* ass;

  public:
    Core::AbstractType::PtrType ret;

    BoolEval(Core::AbstractType* b1, Visitable* op, ExprAssembler* ass);
    BoolEval(Core::AbstractType* b1, Core::AbstractType* b2, Visitable* op,
        ExprAssembler* ass);

    void visitInit(Init* p);
    void visitNoInit(NoInit* p);
    void visitEVar(EVar* p);
    void visitRet(Ret* p);

    void visitNot(Not* p);
    void visitEQU(EQU* p);
    void visitNE(NE* p);

    void visitEAnd(EAnd* p);
    void visitEOr(EOr* p);
};



}

#endif
