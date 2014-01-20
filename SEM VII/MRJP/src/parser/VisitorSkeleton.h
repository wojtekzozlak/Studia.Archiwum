#ifndef SKELETON_HEADER
#define SKELETON_HEADER
/* You might want to change the above name. */

#include "Absyn.H"

#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>


class VisitorSkeleton : public Visitor
{
private:
  void notImplementedError();

public:
  void visitLProgram(LProgram* p);
  void visitTopDef(TopDef* p);
  void visitExt(Ext* p);
  void visitInDef(InDef* p);
  void visitFArg(FArg* p);
  void visitSBlock(SBlock* p);
  void visitStmt(Stmt* p);
  void visitItem(Item* p);
  void visitType(Type* p);
  void visitExpr(Expr* p);
  void visitAddOp(AddOp* p);
  void visitMulOp(MulOp* p);
  void visitRelOp(RelOp* p);
  void visitProgram(Program* p);
  void visitFnDef(FnDef* p);
  void visitFun(Fun* p);
  void visitFunction(Function* p);
  void visitClsDef(ClsDef* p);
  void visitClsExt(ClsExt* p);
  void visitClsNotExt(ClsNotExt* p);
  void visitMethDef(MethDef* p);
  void visitAtrDef(AtrDef* p);
  void visitArg(Arg* p);
  void visitBlock(Block* p);
  void visitEmpty(Empty* p);
  void visitBStmt(BStmt* p);
  void visitDecl(Decl* p);
  void visitNoInit(NoInit* p);
  void visitInit(Init* p);
  void visitAss(Ass* p);
  void visitIncr(Incr* p);
  void visitDecr(Decr* p);
  void visitRet(Ret* p);
  void visitVRet(VRet* p);
  void visitCond(Cond* p);
  void visitCondElse(CondElse* p);
  void visitWhile(While* p);
  void visitFor(For* p);
  void visitSExp(SExp* p);
  void visitInt(Int* p);
  void visitStr(Str* p);
  void visitBool(Bool* p);
  void visitVoid(Void* p);
  void visitArray(Array* p);
  void visitClass(Class* p);
  void visitEVar(EVar* p);
  void visitEApp(EApp* p);
  void visitELitInt(ELitInt* p);
  void visitELitTrue(ELitTrue* p);
  void visitELitFalse(ELitFalse* p);
  void visitENull(ENull* p);
  void visitEString(EString* p);
  void visitENewArray(ENewArray* p);
  void visitENewArrayGet(ENewArrayGet* p);
  void visitENewObj(ENewObj* p);
  void visitECast(ECast* p);
  void visitEGetArray(EGetArray* p);
  void visitEAttr(EAttr* p);
  void visitNeg(Neg* p);
  void visitNot(Not* p);
  void visitEMul(EMul* p);
  void visitEAdd(EAdd* p);
  void visitERel(ERel* p);
  void visitEAnd(EAnd* p);
  void visitEOr(EOr* p);
  void visitPlus(Plus* p);
  void visitMinus(Minus* p);
  void visitTimes(Times* p);
  void visitDiv(Div* p);
  void visitMod(Mod* p);
  void visitLTH(LTH* p);
  void visitLE(LE* p);
  void visitGTH(GTH* p);
  void visitGE(GE* p);
  void visitEQU(EQU* p);
  void visitNE(NE* p);
  void visitListInDef(ListInDef* p);
  void visitListTopDef(ListTopDef* p);
  void visitListFArg(ListFArg* p);
  void visitListStmt(ListStmt* p);
  void visitListItem(ListItem* p);
  void visitListExpr(ListExpr* p);
  void visitListIdent(ListIdent* p);

  void visitInteger(Integer x);
  void visitChar(Char x);
  void visitDouble(Double x);
  void visitString(String x);
  void visitIdent(Ident x);

  virtual ~VisitorSkeleton() {};
};


#endif
