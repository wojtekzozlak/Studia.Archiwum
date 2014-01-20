#include "Environment.h"
#include <iostream>

namespace Core {

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


AbstractScope::EnumeratedVar BlockScope::get(const std::string& var) {
  AbstractScope::VarsMap::iterator it = vars.find(var);
  if (it != vars.end()) return it->second;
  else return parent_scope->get(var);
};


void BlockScope::define(const std::string& var, AbsPtrType el) {
  int i = nextIx();
  vars.insert(std::make_pair(var, std::make_pair(i, el)));
  defs.insert(var);
};


bool BlockScope::isDefined(std::string& var) {
  AbstractScope::VarsSet::iterator it = defs.find(var);
  return (it != defs.end());
};


int BlockScope::nextIx() {
  return parent_scope->nextIx();
};


int BlockScope::maxIx() {
  return parent_scope->nextIx();
};


int BlockScope::nextLab() {
  return parent_scope->nextLab();
};

// BlockScope }}}



// {{{ ClassScope

ClassScope::ClassScope(AbstractScope::PtrType scope, ClassType* cls)
    : parent_scope(scope), cls(cls) {
  assert(cls != NULL);
  assert(scope.get() != NULL);
};


AbstractScope::EnumeratedVar ClassScope::get(const std::string& var) {
  AbsPtrType ret = cls->getAttr(var);
  if (ret.get() != NULL)
    return ret->getType() != FUNCTION ?
      std::make_pair(FIELD_VAR, ret->toLValue()) :
      std::make_pair(METHOD_VAR, ret);
  else return parent_scope->get(var);
};


int ClassScope::nextIx() {
  return parent_scope->nextIx();
};


int ClassScope::maxIx() {
  return parent_scope->maxIx();
};


int ClassScope::nextLab() {
  return parent_scope->nextLab();
};

// ClassScope }}}



// {{{ GlobalScope

GlobalScope::GlobalScope(Environment* env) : env(env), locals(0), labels(0) {};


AbstractScope::EnumeratedVar GlobalScope::get(const std::string& var) {
  Environment::FunMap::iterator it = env->fun.find(var);
  if (it != env->fun.end())
    return std::make_pair(FUNCTION_VAR,
      boost::dynamic_pointer_cast<AbstractType>(it->second));
  else return std::make_pair(EMPTY_VAR, AbsPtrType());
};


int GlobalScope::nextIx() {
  return locals++;
};


int GlobalScope::maxIx() {
  return locals;
}

int GlobalScope::nextLab() {
  return labels++;
};

// GlobalScope }}}

}
