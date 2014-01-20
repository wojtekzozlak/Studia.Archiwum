/****************************************************************************
 * Assembler.h
 *
 * Assembler programów dla x86.
 ****************************************************************************/
#ifndef _X86_BASE_
#define _X86_BASE_

#include <map>
#include "core/Types.h"
#include "core/Environment.h"
#include "parser/VisitorSkeleton.h"

namespace x86 {

typedef std::map<int, int> OffsetMapping;

/* Słownik znanych ciągów znaków występujących w programie. */
class ConstStringMapping {
  private:
    typedef std::map<std::string, int> MapType;
    std::string escape(const std::string& str);
    MapType map;
    int next;

  public:
    ConstStringMapping();
    int get(std::string str);
    void produce(std::ostream& stream);
};


/* Asembler programów. */
class Assembler {
  private:
    Core::Environment* env;

    void assemblyVTable(Core::ClassType::PtrType cls);
    void assemblyFunction(Core::FunType::PtrType fun,
      ConstStringMapping* strings);
    void assemblyMethod(Core::FunType::PtrType fun,
      Core::ClassType::PtrType cls, ConstStringMapping* strings);

  public:
    Assembler(Core::Environment* env);

    void assembly();
};


/* Pomocnik do wyciągania rozmiarów typów. */
class TypeSize : public Core::TypeVisitor {
  public:
    static int getSize(Core::AbstractType* t);

  private:
    int retval;

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

/* Środowisko. */
struct AssemblyEnv {
  Core::Environment* env;
  std::string fun_name;
  std::ostream& stream;
  OffsetMapping offset_mapping;
  ConstStringMapping* strings;

  AssemblyEnv(Core::Environment* env, std::string fun_name,
      std::ostream& stream, ConstStringMapping* strings);
};


/* Asembler instrukcji. */
class StmtAssembler : public VisitorSkeleton {
  Core::BlockScope::PtrType scope;
  AssemblyEnv* ass_env;

  int sp;
  Core::AbstractType::Ptr declBuf;

  public:
    StmtAssembler(Core::BlockScope::PtrType scope, AssemblyEnv* ass_env,
        int sp);

    void visitListStmt(ListStmt* p);
    void visitBlock(Block* p);
    void visitBStmt(BStmt* p);
    void visitEmpty(Empty* p);

    void visitFor(For* p);

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
  void binaryExpr(Expr* arg1, Expr* arg2, Visitable* op);

  void crystalizeBool(std::string t, std::string f, std::string e);

  public:
    Core::BlockScope::PtrType scope;
    AssemblyEnv* ass_env;

    Core::FunType* fun;
    bool lvalue;
    std::string dest;
    std::string arg1;
    std::string arg2;
    std::string if_true;
    std::string if_false;
    int sp;


    ExprAssembler& retLValue();
    void eval(Visitable* p, std::string dest, int sp);
    void eval(Visitable* p, std::string if_true, std::string if_false, int sp);

    ExprAssembler(Core::BlockScope::PtrType scope, AssemblyEnv* ass_env);

    void visitENewObj(ENewObj* p);
    void visitECast(ECast* p);
    void visitENull(ENull* p);

    void visitENewArray(ENewArray* p);
    void visitENewArrayGet(ENewArrayGet* p);
    void visitEGetArray(EGetArray* p);

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
    void visitEAttr(EAttr* p);
};


}

#endif
