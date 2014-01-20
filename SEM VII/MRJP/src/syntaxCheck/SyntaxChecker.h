/****************************************************************************
 * SyntaxCheck.h
 *
 * Klasy odpowiadające za sprawdzanie poprawności składniowej programu.
 *
 * Sprawdzenie przebiega w 3 przebiegach:
 * - Zbierane są nazwy klas i nadklas w programie (w celu poznania wszystkich
 *   nazw typów).
 * - Zbierane są definicje funkcji i metod klasowych.
 * - Sprawdzana jest poprawność ciał wszystkich funkcji i metod.
 *
 * Napotkane błędy wysyłane są na standardowe wyjście błędów, a wynik
 * sprawdzenia dostępny jest w zmiennej globalnej `parsing_successful`.
 ****************************************************************************/
#ifndef _SYNTAX_CHECKER_
#define _SYNTAX_CHECKER_

#include <stack>
#include <set>
#include <boost/shared_ptr.hpp>
#include "parser/VisitorSkeleton.h"
#include "core/Environment.h"
#include "core/Types.h"


namespace SyntaxCheck {

using namespace Core;

  extern bool parsing_successful;

  /* Klasa sprawdzająca poprawność programu za pomocą pozostałych klas. */
  class SyntaxChecker : public VisitorSkeleton {
    private:
      void checkMethod(FunType::PtrType fun, ClassType::PtrType cls);
      void checkFunction(FunType::PtrType fun);

    public:
      Environment env;

      void visitProgram(Program* p);
  };

  /* Klasa sprawdzająca poprawność instrukcji. */
  class StmtChecker : public VisitorSkeleton {
    private:
      Environment* env;
      AbsPtrType ret_type;
      BlockScope::PtrType scope;
      AbsPtrType decl_buf;
      std::stack<Stmt*> patched;

      void maybePatchTree(Stmt** old);

    public:
      bool reachable_return;

      StmtChecker(Environment* env, AbsPtrType ret_type,
          BlockScope::PtrType scope);

      void visitListStmt(ListStmt* p);
      void visitEmpty(Empty* p);
      void visitSExp(SExp* p);
      void visitVRet(VRet* p);
      void visitRet(Ret* p);

      void visitIncr(Incr* p);
      void visitDecr(Decr* p);
      void visitWhile(While* p);
      void visitFor(For* p);
      void visitCondElse(CondElse* p);
      void visitCond(Cond* p);

      void visitDecl(Decl* p);
      void visitListItem(ListItem* p);
      void visitNoInit(NoInit* p);
      void visitInit(Init* p);
      void visitAss(Ass* p);

      void visitBStmt(BStmt* p);
      void visitBlock(Block* p);
  };


  /* Klasa sprawdzająca poprawność wyrażeń. */
  class ExprChecker : public VisitorSkeleton {
    private:
      BlockScope::PtrType scope;
      Environment* env;
      std::stack<AbsPtrType> eval_stack;
      Expr** expr_p;

      static void maybeFoldConstants(Expr** p);
      void checkUnaryExpr(Expr* p, Expr** arg);
      void checkBinaryExpr(Expr* p, Visitable* op, Expr** arg1, Expr** arg2);
      void checkExprError(Expr *p, AbsPtrType& res);

    public:
      bool expr_failure;

      static AbsPtrType eval(BlockScope::PtrType scope, Environment* env,
          Expr** expr_p);
      ExprChecker(BlockScope::PtrType scope, Environment* env);
      AbsPtrType get();

      void visitEVar(EVar* p);
      void visitENewObj(ENewObj* p);
      void visitENewArray(ENewArray* p);
      void visitENewArrayGet(ENewArrayGet* p);
      void visitEGetArray(EGetArray* p);
      void visitECast(ECast* p);
      void visitEAttr(EAttr* p);
      void visitEApp(EApp* p);

      void visitELitInt(ELitInt* p);
      void visitELitTrue(ELitTrue* p);
      void visitELitFalse(ELitFalse* p);
      void visitENull(ENull* p);
      void visitEString(EString* p);

      void visitNeg(Neg* p);
      void visitNot(Not* p);
      void visitEMul(EMul* p);
      void visitEAdd(EAdd* p);
      void visitERel(ERel* p);
      void visitEAnd(EAnd* p);
      void visitEOr(EOr* p);
  };


  /* Klasa zbierająca informacje o nazwach klas i ich nadklasach. */
  class ClassTreeCollector : public VisitorSkeleton {
    protected:
      Environment* env;

    public:
      ClassTreeCollector(Environment* env);

      void visitListTopDef(ListTopDef* p);
      void visitFnDef(FnDef* p);
      void visitClsDef(ClsDef* p);
  };


  /* Klasa zbierająca informacje o atrybutach i metodach klas. */
  class ClassBodyCollector : public ClassTreeCollector {
    private:
      ClassType::PtrType cls_buf;

    public:
      ClassBodyCollector(Environment* env);

      void visitListInDef(ListInDef* p);
      void visitClsDef(ClsDef* p);
      void visitMethDef(MethDef* p);
      void visitAtrDef(AtrDef* p);
  };


  /* Klasa zbierające informacje o funkcjach w programie. */
  class FunctionsCollector : public VisitorSkeleton {
    Environment* env;

    public:
      FunctionsCollector(Environment* env);
      void visitListTopDef(ListTopDef* p);
      void visitFnDef(FnDef* p);
      void visitClsDef(ClsDef* p);
  };


  /* Klasa sprawdzająca poprawność nagłówka funkcji / metody. */
  class FunctionCollector : public VisitorSkeleton {
    Environment* env;
    FunType::PtrType fun;
    std::set<std::string> names_buf;

    public:
      bool good;
      FunctionCollector(Environment* env);

      FunType::PtrType get();
      void visitFun(Fun *p);
      void visitListFArg(ListFArg* p);
      void visitArg(Arg *p);
  };


  /* Klasa sprawdzająca poprawność typu. */
  class TypeCollector : public VisitorSkeleton {
    Environment* env;
    AbsPtrType val;

    public:
      static AbsPtrType eval(Environment* env, Type* t);
      TypeCollector(Environment* env);

      AbsPtrType get();
      void visitInt(Int* p);
      void visitStr(Str* p);
      void visitBool(Bool* p);
      void visitVoid(Void* p);
      void visitClass(Class* p);
      void visitArray(Array* p);
  };
}

#endif
