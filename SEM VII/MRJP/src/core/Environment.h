/****************************************************************************
 * Environment.h
 *
 * Klasa opisująca typy i funkcje w programie, oraz klasy określające zasięgi
 * zmiennych w pewnym momencie wykonania programu.
 *
 * Zasady działania zasięgów opisane są w pliku README.
 ****************************************************************************/
#ifndef _SYNTAXCHECK_ENVIRONMENT_
#define _SYNTAXCHECK_ENVIRONMENT_

#include <boost/shared_ptr.hpp>
#include <map>
#include <set>
#include <vector>
#include "Types.h"

namespace Core {


/* Klasa środowiska, wzbogacana o definicje metod i klas w programie.
 *
 * Początkowo zna predefiniowane funkcje języka. */
class Environment {
  private:
    void addDefaultFunctions();

  public:
    typedef std::map<std::string, FunType::PtrType> FunMap;
    typedef std::map<std::string, ClassType::PtrType> ClsMap;

    FunMap fun;
    ClsMap cls;

    Environment();

    void insertFunction(FunType::PtrType fun);
    void debug();
};


enum SpecialVar {
  FIELD_VAR = -1,
  METHOD_VAR = -2,
  FUNCTION_VAR = -3,
  EMPTY_VAR = -4
};


/* Abstrakcyjny zasięg, który umie zwrócić typ pod danym identyfikatorem. */
class AbstractScope {
  public:
    typedef std::pair<int, AbstractType::Ptr> EnumeratedVar;
    typedef std::map<std::string, EnumeratedVar> VarsMap;
    typedef std::set<std::string> VarsSet;
    typedef boost::shared_ptr<AbstractScope> PtrType;

    virtual EnumeratedVar get(const std::string& var) = 0;
    virtual int nextIx() = 0;
    virtual int maxIx() = 0;
    virtual int nextLab() = 0;
};


/* Zasięg blokowych, dla zmiennych deklarowanych w programie. */
class BlockScope : public AbstractScope {
  private:
    AbstractScope::PtrType parent_scope;

  public:
    typedef boost::shared_ptr<BlockScope> PtrType;
    AbstractScope::VarsMap vars;
    AbstractScope::VarsSet defs;

    BlockScope(AbstractScope::PtrType scope);

    AbstractScope::EnumeratedVar get(const std::string& var);
    void define(const std::string& var, AbsPtrType el);
    bool isDefined(std::string& var);
    int nextIx();
    int maxIx();
    int nextLab();
};


/* Zasięg klasowy, dla atrybutów i metod klasy. */
class ClassScope : public AbstractScope {
  private:
    AbstractScope::PtrType parent_scope;
    ClassType* cls;

  public:
    ClassScope(AbstractScope::PtrType scope, ClassType* cls);

    AbstractScope::EnumeratedVar get(const std::string& var);
    int nextIx();
    int maxIx();
    int nextLab();
};


/* Zasięg globalny dla funkcji zdefiniowanych w programie. */
class GlobalScope : public AbstractScope {
  private:
    Environment* env;

  public:
    int locals;
    int labels;

    GlobalScope(Environment* env);

    AbstractScope::EnumeratedVar get(const std::string& var);
    int nextIx();
    int maxIx();
    int nextLab();
};

}

#endif
