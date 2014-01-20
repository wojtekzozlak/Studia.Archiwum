#include "Environment.h"
#include <iostream>

namespace SyntaxCheck {

void Environment::addDefaultFunctions() {
  // void printInt(int);
  FunType::PtrType printInt(new FunType("printInt"));
  printInt->args.push_back(AbsPtrType(new IntType()));
  printInt->ret_type = AbsPtrType(new VoidType());
  insertFunction(printInt);

  // void printString(string);
  FunType::PtrType printString(new FunType("printString"));
  printString->args.push_back(AbsPtrType(new StringType()));
  printString->ret_type = AbsPtrType(new VoidType());
  insertFunction(printString);

  // void error();
  FunType::PtrType error(new FunType("error"));
  error->ret_type = AbsPtrType(new VoidType());
  insertFunction(error);

  // int readInt();
  FunType::PtrType readInt(new FunType("readInt"));
  readInt->ret_type = AbsPtrType(new IntType());
  insertFunction(readInt);

  // string readString();
  FunType::PtrType readString(new FunType("readString"));
  readString->ret_type = AbsPtrType(new StringType());
  insertFunction(readString);
};


Environment::Environment() {
  addDefaultFunctions();
};


void Environment::insertFunction(FunType::PtrType fun) {
  this->fun.insert(std::make_pair(fun->name, fun));
};


void Environment::debug() {
  std::cerr << "Functions: " << std::endl;
  FunMap::iterator it;
  for (it = fun.begin(); it != fun.end(); it++) {
    std::cerr << "\t" << it->first << ": " << it->second->toString() <<
        std::endl;
  }

  std::cerr << "Classes: " << std::endl;
  ClsMap::iterator cit;
  for (cit = cls.begin(); cit != cls.end(); cit++) {
    cit->second->debug();
  }
};


// {{{ BlockScope

BlockScope::BlockScope(AbstractScope::PtrType scope) : parent_scope(scope) {
  assert(scope.get() != NULL);
};


AbstractScope::EnumeratedVar BlockScope::get(std::string& var) {
  AbstractScope::VarsMap::iterator it = vars.find(var);
  if (it != vars.end()) return it->second;
  else return parent_scope->get(var);
};


void BlockScope::define(std::string& var, AbsPtrType el) {
  vars.insert(std::make_pair(var, std::make_pair(0, el)));
  defs.insert(var);
};


bool BlockScope::isDefined(std::string& var) {
  AbstractScope::VarsSet::iterator it = defs.find(var);
  return (it != defs.end());
};

// BlockScope }}}



// {{{ FunctionScope

ClassScope::ClassScope(AbstractScope::PtrType scope, ClassType* cls)
    : parent_scope(scope), cls(cls) {
  assert(cls != NULL);
  assert(scope.get() != NULL);
};


AbstractScope::EnumeratedVar ClassScope::get(std::string& var) {
  AbsPtrType ret = cls->getAttr(var);
  if (ret.get() != NULL) return ret->toLValue();
  else return parent_scope->get(var);
};

// FunctionScope }}}



// {{{ GlobalScope

GlobalScope::GlobalScope(Environment* env) : env(env) {};


AbstractScope::EnumeratedVar GlobalScope::get(std::string& var) {
  Environment::FunMap::iterator it = env->fun.find(var);
  if (it != env->fun.end())
    return boost::dynamic_pointer_cast<AbstractType>(it->second);
  else return std::make_pair(-1, AbsPtrType());
};

// GlobalScope }}}

}
