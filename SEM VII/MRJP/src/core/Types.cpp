#include <sstream>
#include "Types.h"
#include "syntaxCheck/Errors.h"

namespace Core {

// {{{ AbstractType

bool AbstractType::isA(AbstractType& el) {
  return this->getType() == el.getType();
};

// AbstractType }}}


// {{{ NullType

AbsPtrType NullType::obj() {
  static AbsPtrType obj(new NullType());
  return obj;
}


TypeType NullType::getType()
{ return NULLY; };


std::string& NullType::toString() {
  static std::string str("null");
  return str;
};


AbsPtrType NullType::toLValue() {
  assert(false);
};


AbsPtrType NullType::toRValue() {
  return NullType::obj();
};


void NullType::accept(TypeVisitor* v) {
  v->visitNull(this);
};

// NullType }}}

// {{{ StringType

AbsPtrType StringType::obj() {
  static AbsPtrType obj(new StringType());
  return obj;
};


TypeType StringType::getType()
{ return STRING; };


std::string& StringType::toString() {
  static std::string str("string");
  return str;
};


AbsPtrType StringType::toLValue() {
  AbsPtrType ret(new StringType(*this));
  ret->lvalue = true;
  return ret;
};


AbsPtrType StringType::toRValue() {
  AbsPtrType ret(new StringType(*this));
  ret->lvalue = false;
  return ret;
};


void StringType::accept(TypeVisitor* v) {
  v->visitString(this);
};

// StringType }}}

// {{{ BoolType

BoolType::BoolType() : value(false), unknown(true) {};


BoolType::BoolType(bool value) : value(value), unknown(false) {};


TypeType BoolType::getType()
{ return BOOL; };

AbsPtrType BoolType::obj() {
  static AbsPtrType obj(new BoolType());
  return obj;
};


std::string& BoolType::toString() {
  std::ostringstream ss;
  ss << "Bool";
  if (!this->unknown) ss << ":" << this->value;
  str = ss.str();
  return str;
};


AbsPtrType BoolType::toLValue() {
  AbsPtrType ret(new BoolType(*this));
  ret->lvalue = true;
  return ret;
};


AbsPtrType BoolType::toRValue() {
  AbsPtrType ret(new BoolType(*this));
  ret->lvalue = false;
  return ret;
};


void BoolType::accept(TypeVisitor* v) {
  v->visitBool(this);
}

// BoolType }}}



// {{{ IntType

IntType::IntType() : value(0), unknown(true) {};


IntType::IntType(long int value) : value(value), unknown(false) {};


AbsPtrType IntType::obj() {
  static AbsPtrType obj(new IntType());
  return obj;
};


TypeType IntType::getType()
{ return INT; }


std::string& IntType::toString() {
  std::ostringstream ss;
  ss << "Int";
  if (!this->unknown) ss << ":" << this->value;
  str = ss.str();
  return str;
};


AbsPtrType IntType::toLValue() {
  AbsPtrType ret(new IntType(*this));
  ret->lvalue = true;
  return ret;
};


AbsPtrType IntType::toRValue() {
  AbsPtrType ret(new IntType(*this));
  ret->lvalue = false;
  return ret;
};


void IntType::accept(TypeVisitor* v) {
  v->visitInt(this);
};

// IntType }}}



// {{{ InvalidType

InvalidType::InvalidType() : reason("") {
  lvalue = false;
};


InvalidType::InvalidType(std::string reason) : reason(reason) {
  lvalue = false;
};


InvalidType::InvalidType(std::string reason, AbstractType *arg1) {
  std::ostringstream ss;
  ss << reason << " '" << arg1->toString() << "'";
  this->reason = ss.str();
};


InvalidType::InvalidType(std::string reason, AbstractType *arg1,
    AbstractType *arg2) {
  std::ostringstream ss;
  ss << reason << " '" << arg1->toString() << "' and '" << arg2->toString()
     << "'";
  this->reason = ss.str();
};


AbsPtrType InvalidType::withStdReason(Visitable *op, AbstractType* arg1) {
  std::string& str = SyntaxCheck::GeneralError::obj().getMsg(op);
  return InvalidType::PtrType(new InvalidType(str, arg1));
}


AbsPtrType InvalidType::withStdReason(Visitable *op, AbstractType* arg1,
    AbstractType* arg2) {
  std::string& str = SyntaxCheck::GeneralError::obj().getMsg(op);
  return InvalidType::PtrType(new InvalidType(str, arg1, arg2));
}


AbsPtrType InvalidType::obj() {
  static AbsPtrType obj(new InvalidType());
  return obj;
};


TypeType InvalidType::getType()
{ return INVALID; };


std::string& InvalidType::toString() {
  static std::string str("InvalidType");
  return str;
};


AbsPtrType InvalidType::toLValue() {
  assert(false);
};


AbsPtrType InvalidType::toRValue() {
  return InvalidType::obj();
};


void InvalidType::accept(TypeVisitor* v) {
  v->visitInvalid(this);
};

// InvalidType }}}


// {{{ VoidType

TypeType VoidType::getType() {
  return VOID;
}


AbsPtrType VoidType::obj() {
  static AbsPtrType obj(new VoidType());
  return obj;
};


std::string& VoidType::toString() {
  static std::string str("void");
  return str;
};


void VoidType::accept(TypeVisitor* v) {
  v->visitVoid(this);
};

// VoidType }}}


// {{{ FunType

FunType::FunType(std::string name) : name(name), body(NULL)
{};


TypeType FunType::getType() {
  return FUNCTION;
};


bool FunType::isA(AbstractType& el) {
  if (el.getType() != getType()) return false;

  FunType* fun_el = dynamic_cast<FunType*>(&el);
  assert(fun_el != NULL);

  if (!ret_type->isA(*(fun_el->ret_type))) return false;

  if (args.size() != fun_el->args.size()) return false;

  for (int i = 0; i < args.size(); i++) {
    if (!args[i]->isA(*(fun_el->args[i]))) return false;
  }


  return true;
};


std::string& FunType::toString() {
  assert(ret_type.get() != NULL);
  int i;

  std::ostringstream ss;
  ss << ret_type->toString() << " (";
  for (i = 0; i < args.size(); i++) {
    ss << args[i]->toString();
    if (i != args.size() - 1) ss << ", ";
  }
  ss << ")";
  str = ss.str();
  return str;
};


void FunType::accept(TypeVisitor* v) {
  v->visitFun(this);
};

// FunType }}}


// {{{ ClassType

AbsPtrType ClassType::getAttr(std::string name) {
  AttrMap::iterator it = attrs.find(name);

  if (it != attrs.end()) return it->second;
  else if(parent.get() != NULL) return parent->getAttr(name);
  else return AbsPtrType();
};


bool ClassType::isA(AbstractType& el) {
  if (el.getType() != this->getType()) return false;

  ClassType* cls_el = dynamic_cast<ClassType*>(&el);
  assert(cls_el != NULL);

  if (name.compare(cls_el->name) == 0) return true;
  else if (parent.get() != NULL) return parent->isA(el);
  return false;
};


TypeType ClassType::getType() { return CLASS; }


std::string& ClassType::toString() {
  return name;
};


void ClassType::debug() {
  std::cerr << "\t" << this->toString() << std::endl;
  AttrMap::iterator it;
  for (it = attrs.begin(); it != attrs.end(); it++) {
    std::cerr << "\t\t" << it->first << ": " << it->second->toString() <<
std::endl;
  }
};


AbsPtrType ClassType::toLValue() {
  AbsPtrType ret(new ClassType(*this));
  ret->lvalue = true;
  return ret;
};


AbsPtrType ClassType::toRValue() {
  AbsPtrType ret(new ClassType(*this));
  ret->lvalue = false;
  return ret;
};


void ClassType::accept(TypeVisitor* v) {
  v->visitClass(this);
};


int ClassType::getSize() {
  int attributes = 0;
  AttrMap::iterator it;
  for (it = attrs.begin(); it != attrs.end(); it++) {
    if (it->second->getType() != Core::FUNCTION &&
        (parent.get() == NULL ||
        parent->getAttr(it->first).get() == NULL)) {
      attributes++;
    }
  }


  return getParentSize() + attributes * 4;
};


int ClassType::getVTableSize() {
  int methods = 0;
  AttrMap::iterator it;
  for (it = attrs.begin(); it != attrs.end(); it++) {
    if (it->second->getType() == Core::FUNCTION &&
        (parent.get() == NULL ||
        parent->getAttr(it->first).get() == NULL)) {
      methods++;
    }
  }

  return getParentVTableSize() + methods * 4;
};


int ClassType::getParentSize() {
  return parent.get() != NULL ? parent->getSize() : 0;
};


int ClassType::getParentVTableSize() {
  return parent.get() != NULL ? parent->getVTableSize() : 0;
};


void ClassType::initAttrOffsets() {
  int offset = getParentSize();
  AttrMap::iterator it;
  for (it = attrs.begin(); it != attrs.end(); it++) {
    if (it->second->getType() != Core::FUNCTION) {
      attrOffsets.insert(std::make_pair(it->first, offset));
      offset += 4;
    }
  }

  assert(getSize() == offset);
};


void ClassType::initMethOffsets() {
  int offset = getParentVTableSize();
  AttrMap::iterator it;
  for (it = attrs.begin(); it != attrs.end(); it++) {
    if (it->second->getType() == Core::FUNCTION &&
        (parent.get() == NULL ||
        parent->getAttr(it->first).get() == NULL)) {
      methOffsets.insert(std::make_pair(it->first, offset));
      offset += 4;
    }
  }

  assert(getVTableSize() == offset);
};


int ClassType::attrOffset(const std::string attr) {
  if (!attrOffsetsInitialized) {
    initAttrOffsets();
    attrOffsetsInitialized = true;
  }

  OffsetMap::iterator it = attrOffsets.find(attr);
  if (it != attrOffsets.end())
    return it->second;
  else
    return parent->attrOffset(attr);
};


int ClassType::methOffset(const std::string meth) {
  if (!methOffsetsInitialized) {
    initMethOffsets();
    methOffsetsInitialized = true;
  }

  OffsetMap::iterator it = methOffsets.find(meth);
  if (it != methOffsets.end())
    return it->second;
  else
    return parent->methOffset(meth);
};


std::vector<std::string> ClassType::produceVTable() {
  if (!methOffsetsInitialized) {
    initMethOffsets();
    methOffsetsInitialized = true;
  }

  std::vector<std::string> vtable;
  if (parent.get() != NULL)
    vtable = parent->produceVTable();
  vtable.resize(getVTableSize() / 4);

  OffsetMap::iterator it;
  for (it = methOffsets.begin(); it != methOffsets.end(); it++) {
    vtable[it->second / 4] = it->first;
  }
  return vtable;
};


std::vector<int> ClassType::stringOffsets() {
  std::vector<int> strings;
  if (parent.get() != NULL)
    strings = parent->stringOffsets();

  AttrMap::iterator it;
  for (it = attrs.begin(); it != attrs.end(); it++) {
    if (it->second->getType() == Core::STRING) {
      strings.push_back(attrOffset(it->first));
    }
  }
  return strings;
};

// ClassType }}}



// {{{ ArrayType

ArrayType::ArrayType(AbsPtrType ptr)
     : length(0), undefined(true), elementsType(ptr) {
  assert(ptr->getType() != INVALID);
  assert(ptr->getType() != ARRAY);
  assert(ptr->getType() != VOID);
};


ArrayType::ArrayType(AbsPtrType ptr, int l)
    : length(l), undefined(false), elementsType(ptr) {
  assert(ptr->getType() != INVALID);
  assert(ptr->getType() != ARRAY);
  assert(ptr->getType() != VOID);
};


TypeType ArrayType::getType() {
  return ARRAY;
};


bool ArrayType::isA(AbstractType& el) {
  if (el.getType() != this->getType()) return false;

  ArrayType* arr_el = dynamic_cast<ArrayType*>(&el);
  assert(arr_el != NULL);
  return this->elementsType->isA(*arr_el->elementsType);
};


std::string& ArrayType::toString() {
  std::ostringstream ss;
  ss << elementsType->toString() << "[";
  if (!undefined) ss << length;
  ss << "]";
  str = ss.str();
  return str;
};


AbsPtrType ArrayType::toLValue() {
  AbsPtrType ret(new ArrayType(*this));
  ret->lvalue = true;
  return ret;
};


AbsPtrType ArrayType::toRValue() {
  AbsPtrType ret(new ArrayType(*this));
  ret->lvalue = false;
  return ret;
};


void ArrayType::accept(TypeVisitor* v) {
  v->visitArray(this);
};

// ArrayType }}}


// {{{ TypeVisitor

void TypeVisitor::visitInt(IntType* t) { assert(false); }
void TypeVisitor::visitVoid(VoidType* t) { assert(false); }
void TypeVisitor::visitString(StringType* t) { assert(false); }
void TypeVisitor::visitInvalid(InvalidType* t) { assert(false); }
void TypeVisitor::visitBool(BoolType* t) { assert(false); }
void TypeVisitor::visitFun(FunType* t) { assert(false); }
void TypeVisitor::visitArray(ArrayType* t) { assert(false); }
void TypeVisitor::visitClass(ClassType* t) { assert(false); }
void TypeVisitor::visitNull(NullType* t) { assert(false); }


// TypeVisitor }}}

};
