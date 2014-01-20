/*** BNFC-Generated Visitor Design Pattern VisitorSkeleton. ***/
/* This implements the common visitor design pattern.
   Note that this method uses Visitor-traversal of lists, so
   List->accept() does NOT traverse the list. This allows different
   algorithms to use context information differently. */

#include "VisitorSkeleton.h"
#include <iostream>

void VisitorSkeleton::notImplementedError() {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: not implemented visitor method:\n");
  backtrace_symbols_fd(array, size, 2);
  exit(1);
}

void VisitorSkeleton::visitLProgram(LProgram* t)  //abstract class
{ notImplementedError(); }
void VisitorSkeleton::visitTopDef(TopDef* t)  //abstract class
{ notImplementedError(); }
void VisitorSkeleton::visitExt(Ext* t)  //abstract class
{ notImplementedError(); }
void VisitorSkeleton::visitInDef(InDef* t)  //abstract class
{ notImplementedError(); }
void VisitorSkeleton::visitFArg(FArg* t)  //abstract class
{ notImplementedError(); }
void VisitorSkeleton::visitSBlock(SBlock* t)  //abstract class
{ notImplementedError(); }
void VisitorSkeleton::visitStmt(Stmt* t)  //abstract class
{ notImplementedError(); }
void VisitorSkeleton::visitItem(Item* t)  //abstract class
{ notImplementedError(); }
void VisitorSkeleton::visitType(Type* t)  //abstract class
{ notImplementedError(); }
void VisitorSkeleton::visitExpr(Expr* t)  //abstract class
{ notImplementedError(); }
void VisitorSkeleton::visitAddOp(AddOp* t)  //abstract class
{ notImplementedError(); }
void VisitorSkeleton::visitMulOp(MulOp* t)  //abstract class
{ notImplementedError(); }
void VisitorSkeleton::visitRelOp(RelOp* t)  //abstract class
{ notImplementedError(); }

void VisitorSkeleton::visitProgram(Program *program)
{ notImplementedError(); }

void VisitorSkeleton::visitFnDef(FnDef *fndef)
{ notImplementedError(); }

void VisitorSkeleton::visitFun(Fun *fun)
{ notImplementedError(); }

void VisitorSkeleton::visitFunction(Function *function)
{ notImplementedError(); }

void VisitorSkeleton::visitClsDef(ClsDef *clsdef)
{ notImplementedError(); }

void VisitorSkeleton::visitClsExt(ClsExt *clsext)
{ notImplementedError(); }

void VisitorSkeleton::visitClsNotExt(ClsNotExt *clsnotext)
{ notImplementedError(); }

void VisitorSkeleton::visitMethDef(MethDef *methdef)
{ notImplementedError(); }

void VisitorSkeleton::visitAtrDef(AtrDef *atrdef)
{ notImplementedError(); }

void VisitorSkeleton::visitArg(Arg *arg)
{ notImplementedError(); }

void VisitorSkeleton::visitBlock(Block *block)
{ notImplementedError(); }

void VisitorSkeleton::visitEmpty(Empty *empty)
{ notImplementedError(); }

void VisitorSkeleton::visitBStmt(BStmt *bstmt)
{ notImplementedError(); }

void VisitorSkeleton::visitDecl(Decl *decl)
{ notImplementedError(); }

void VisitorSkeleton::visitAss(Ass *ass)
{ notImplementedError(); }

void VisitorSkeleton::visitIncr(Incr *incr)
{ notImplementedError(); }

void VisitorSkeleton::visitDecr(Decr *decr)
{ notImplementedError(); }

void VisitorSkeleton::visitRet(Ret *ret)
{ notImplementedError(); }

void VisitorSkeleton::visitVRet(VRet *vret)
{ notImplementedError(); }

void VisitorSkeleton::visitCond(Cond *cond)
{ notImplementedError(); }

void VisitorSkeleton::visitCondElse(CondElse *condelse)
{ notImplementedError(); }

void VisitorSkeleton::visitWhile(While *w)
{ notImplementedError(); }

void VisitorSkeleton::visitFor(For *f)
{ notImplementedError(); }

void VisitorSkeleton::visitSExp(SExp *sexp)
{ notImplementedError(); }

void VisitorSkeleton::visitNoInit(NoInit *noinit)
{ notImplementedError(); }

void VisitorSkeleton::visitInit(Init *init)
{ notImplementedError(); }

void VisitorSkeleton::visitInt(Int *i)
{ notImplementedError(); }

void VisitorSkeleton::visitStr(Str *str)
{ notImplementedError(); }

void VisitorSkeleton::visitBool(Bool *b)
{ notImplementedError(); }

void VisitorSkeleton::visitVoid(Void *v)
{ notImplementedError(); }

void VisitorSkeleton::visitArray(Array *table)
{ notImplementedError(); }

void VisitorSkeleton::visitClass(Class *cls)
{ notImplementedError(); }

void VisitorSkeleton::visitEVar(EVar *evar)
{ notImplementedError(); }

void VisitorSkeleton::visitEApp(EApp *eapp)
{ notImplementedError(); }

void VisitorSkeleton::visitELitInt(ELitInt *elitint)
{ notImplementedError(); }

void VisitorSkeleton::visitELitTrue(ELitTrue *elittrue)
{ notImplementedError(); }

void VisitorSkeleton::visitELitFalse(ELitFalse *elitfalse)
{ notImplementedError(); }

void VisitorSkeleton::visitENull(ENull *enull)
{ notImplementedError(); }

void VisitorSkeleton::visitEString(EString *estring)
{ notImplementedError(); }

void VisitorSkeleton::visitENewArray(ENewArray *enewarray)
{ notImplementedError(); }

void VisitorSkeleton::visitENewArrayGet(ENewArrayGet *enewarrayget)
{ notImplementedError(); }

void VisitorSkeleton::visitENewObj(ENewObj *enewobj)
{ notImplementedError(); }

void VisitorSkeleton::visitECast(ECast *ecast)
{ notImplementedError(); }

void VisitorSkeleton::visitEGetArray(EGetArray *egetarray)
{ notImplementedError(); }

void VisitorSkeleton::visitEAttr(EAttr *eattr)
{ notImplementedError(); }

void VisitorSkeleton::visitNeg(Neg *neg)
{ notImplementedError(); }

void VisitorSkeleton::visitNot(Not *n)
{ notImplementedError(); }

void VisitorSkeleton::visitEMul(EMul *emul)
{ notImplementedError(); }

void VisitorSkeleton::visitEAdd(EAdd *eadd)
{ notImplementedError(); }

void VisitorSkeleton::visitERel(ERel *erel)
{ notImplementedError(); }

void VisitorSkeleton::visitEAnd(EAnd *eand)
{ notImplementedError(); }

void VisitorSkeleton::visitEOr(EOr *eor)
{ notImplementedError(); }

void VisitorSkeleton::visitPlus(Plus *plus)
{ notImplementedError(); }

void VisitorSkeleton::visitMinus(Minus *minus)
{ notImplementedError(); }

void VisitorSkeleton::visitTimes(Times *times)
{ notImplementedError(); }

void VisitorSkeleton::visitDiv(Div *div)
{ notImplementedError(); }

void VisitorSkeleton::visitMod(Mod *mod)
{ notImplementedError(); }

void VisitorSkeleton::visitLTH(LTH *lth)
{ notImplementedError(); }

void VisitorSkeleton::visitLE(LE *le)
{ notImplementedError(); }

void VisitorSkeleton::visitGTH(GTH *gth)
{ notImplementedError(); }

void VisitorSkeleton::visitGE(GE *ge)
{ notImplementedError(); }

void VisitorSkeleton::visitEQU(EQU *equ)
{ notImplementedError(); }

void VisitorSkeleton::visitNE(NE *ne)
{ notImplementedError(); }

void VisitorSkeleton::visitListInDef(ListInDef* listindef)
{ notImplementedError(); }

void VisitorSkeleton::visitListTopDef(ListTopDef* listtopdef)
{ notImplementedError(); }

void VisitorSkeleton::visitListFArg(ListFArg* listfarg)
{ notImplementedError(); }

void VisitorSkeleton::visitListStmt(ListStmt* liststmt)
{ notImplementedError(); }

void VisitorSkeleton::visitListItem(ListItem* listitem)
{ notImplementedError(); }

void VisitorSkeleton::visitListExpr(ListExpr* listexpr)
{ notImplementedError(); }

void VisitorSkeleton::visitListIdent(ListIdent* listident)
{ notImplementedError(); }

void VisitorSkeleton::visitInteger(Integer x)
{ notImplementedError(); }

void VisitorSkeleton::visitChar(Char x)
{ notImplementedError(); }

void VisitorSkeleton::visitDouble(Double x)
{ notImplementedError(); }

void VisitorSkeleton::visitString(String x)
{ notImplementedError(); }

void VisitorSkeleton::visitIdent(Ident x)
{ notImplementedError(); }

