/****************************************************************************
 * Assembler.h
 *
 * Assembler programów dla JVM.
 ****************************************************************************/
#ifndef _JVM_BASE_
#define _JVM_BASE_

#include <iostream>
#include <stack>
#include "core/Types.h"
#include "core/Environment.h"
#include "parser/VisitorSkeleton.h"
#include "Eval.h"

namespace JVM {


/* Asembler programów. */
class Assembler {
  Core::Environment* env;
  std::string name;

  void assemblyFunction(Core::FunType::PtrType fun, Core::Environment* env);

  public:
    static std::string functionSignature(Core::FunType::PtrType fun);
    Assembler(Core::Environment* env, std::string& name);

    void assembly();
};


/* Dostawca sygnatur typów. */
class TypeSignature : public Core::TypeVisitor {
  std::string str;

  public:
    std::string get();

    TypeSignature(Core::AbstractType::PtrType t);

    void visitInt(Core::IntType* t);
    void visitVoid(Core::VoidType* t);
    void visitString(Core::StringType* t);
    void visitArray(Core::ArrayType* t);
    void visitClass(Core::ClassType* t);
    void visitBool(Core::BoolType* t);
};


/* Asembler wyrażeń. */
class StmtAssembler : public VisitorSkeleton {
  Core::BlockScope::PtrType scope;
  Core::Environment* env;
  std::ostream& stream;
  Core::AbstractType::Ptr declBuf;


  public:
    size_t stackLimit;

    StmtAssembler(Core::BlockScope::PtrType scope, Core::Environment* env,
        std::ostream& stream);

    void visitListStmt(ListStmt* p);
    void visitBlock(Block* p);
    void visitBStmt(BStmt* p);
    void visitEmpty(Empty* p);

    void visitSExp(SExp* p);
    void visitRet(Ret* p);
    void visitVRet(VRet* p);
    void visitDecl(Decl* p);
    void visitNoInit(NoInit* p);
    void visitInit(Init* p);
    void visitAss(Ass* p);

    void visitCond(Cond* p);
    void visitCondElse(CondElse* p);
    void visitWhile(While* p);

    void visitDecr(Decr* p);
    void visitIncr(Incr* p);
};


/* Asembler wyrażeń. */
class ExprAssembler : public VisitorSkeleton {
  void unaryExpr(Visitable* arg, Visitable* op);
  void binaryExpr(Visitable* arg1, Visitable* arg2, Visitable* op);

  public:
    Core::BlockScope::PtrType scope;
    Core::Environment* env;
    std::ostream& stream;
    std::stack<Core::AbstractType::PtrType> evalStack;
    size_t stackLimit;

    void push(Core::AbstractType::PtrType t);
    std::stack<LabelPair*> labelStack;
    LabelPair* nextLabelPair();
    void eval(Expr* p);

    ExprAssembler(Core::BlockScope::PtrType scope, Core::Environment* env,
        std::ostream& stream);

    void visitELitTrue(ELitTrue* p);
    void visitELitFalse(ELitFalse* p);
    void visitELitInt(ELitInt* p);
    void visitEString(EString* p);

    void visitNeg(Neg* p);
    void visitNot(Not* p);
    void visitEMul(EMul* p);
    void visitERel(ERel* p);
    void visitEAdd(EAdd* p);
    void visitEAnd(EAnd* p);
    void visitEOr(EOr* p);

    void visitEVar(EVar* p);
    void visitEApp(EApp* p);
};

}

#endif
