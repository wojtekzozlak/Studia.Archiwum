/****************************************************************************
 * Types.h
 *
 * Definicje klas reprezentujących typy w programie.
 *
 * Typy umieją sprawdzać czy zachodzi między nimi relacja
 * bycia (is-a), określić typ wyniku operacji unarnej / binarnej wykonanej
 * na nim (i jakimś innym typie), powiedzieć czy jest l- czy r-wartością oraz
 * zwrocić obiekt tego samego typu będący l- bądź r-wartością (o ile jest typem
 * który może być l-wartością).
 ****************************************************************************/
#ifndef _SYNTAXCHECKER_TYPES_
#define _SYNTAXCHECKER_TYPES_

#define INVALID_TYPE (InvalidType::obj())
#define THIS_TYPE (AbsPtrType(this))

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <map>
#include <vector>
#include "parser/Absyn.H"

namespace Core {

  enum TypeType {
    BOOL,
    INT,
    CLASS,
    NULLY,
    VOID,
    STRING,
    ARRAY,
    FUNCTION,
    INVALID
  };


  class TypeVisitor;


  /* Klasa abstrakcyjna dla typów */
  class AbstractType {
    public:
      typedef boost::shared_ptr<AbstractType> PtrType;
      typedef boost::shared_ptr<AbstractType> Ptr;
      bool lvalue; // prawda, gdy jest l-wartością

      virtual TypeType getType() = 0;

      virtual bool isA(AbstractType& el);
      virtual std::string& toString() = 0;
      virtual PtrType toLValue() = 0;
      virtual PtrType toRValue() = 0;

      virtual void accept(TypeVisitor* v) = 0;
  };
  typedef AbstractType::PtrType AbsPtrType;


  /* Typ wynikowy dla niepoprawnych operacji. Zamknięty na dowolne operacje
   * (wynikiem też jest typ niepoprawny) */
  class InvalidType : public AbstractType {
    private:
      InvalidType(std::string reason, AbstractType* arg1);
      InvalidType(std::string reason, AbstractType* arg1, AbstractType* arg2);

    public:
      typedef boost::shared_ptr<InvalidType> PtrType;
      std::string reason;

      InvalidType();
      InvalidType(std::string reason);

      static AbsPtrType obj();
      static AbsPtrType withStdReason(Visitable *op, AbstractType* arg1);
      static AbsPtrType withStdReason(Visitable *op, AbstractType* arg1,
          AbstractType* arg2);

      virtual TypeType getType();

      virtual std::string& toString();
      virtual AbsPtrType toLValue();
      virtual AbsPtrType toRValue();

      virtual void accept(TypeVisitor* v);
  };


  /* Typ dla wartości `null`, nie można na nim wykonywać żadnych operacji z
   * wyjątkiem rzutowania */
  class NullType : public InvalidType {
    private:
      NullType() {};

    public:
      static AbsPtrType obj();

      TypeType getType();

      std::string& toString();
      AbsPtrType toLValue();
      AbsPtrType toRValue();

      void accept(TypeVisitor* v);
  };


  /* Typ dla ciągów znaków */
  class StringType : public AbstractType {
    public:
      static AbsPtrType obj();

      TypeType getType();

      std::string& toString();
      AbsPtrType toLValue();
      AbsPtrType toRValue();

      void accept(TypeVisitor* v);
  };


  /* Typ logiczny */
  class BoolType : public AbstractType {
    private:
      std::string str;

    public:
      typedef boost::shared_ptr<BoolType> PtrType;
      bool value;
      bool unknown;

      BoolType();
      BoolType(bool value);

      TypeType getType();
      static AbsPtrType obj();

      std::string& toString();
      AbsPtrType toLValue();
      AbsPtrType toRValue();

      void accept(TypeVisitor* v);
  };


  /* Typ całkowitoliczbowy */
  class IntType : public AbstractType {
    private:
      std::string str;

    public:
      long int value;
      bool unknown;

      IntType();
      IntType(long int value);

      static AbsPtrType obj();
      TypeType getType();

      std::string& toString();
      AbsPtrType toLValue();
      AbsPtrType toRValue();

      void accept(TypeVisitor* v);
  };


  /* Typ pusty dla procedur */
  class VoidType : public InvalidType {
    public:
      static AbsPtrType obj();
      TypeType getType();
      std::string& toString();

      void accept(TypeVisitor* v);
  };


  /* Typ klasowy.
   *
   * Zna definicje atrybutów i metod klasy oraz ewentualną nadklasę. */
  class ClassType : public AbstractType {
    private:
      typedef std::map<std::string, int> OffsetMap;
      std::string str;
      OffsetMap attrOffsets;
      OffsetMap methOffsets;
      bool attrOffsetsInitialized;
      bool methOffsetsInitialized;

      void initAttrOffsets();
      void initMethOffsets();
      int getParentSize();
      int getParentVTableSize();

    public:
      typedef std::map<std::string, AbsPtrType> AttrMap;
      typedef boost::shared_ptr<ClassType> PtrType;

      PtrType parent;
      AttrMap attrs;
      std::string name;

      ClassType(std::string name) : attrOffsetsInitialized(false),
          name(name) {};
      AbsPtrType getAttr(std::string name);
      bool isA(AbstractType& el);
      TypeType getType();
      std::string& toString();
      void debug();

      int getSize();
      int getVTableSize();
      int attrOffset(const std::string attr);
      int methOffset(const std::string meth);
      std::vector<std::string> produceVTable();
      std::vector<int> stringOffsets();

      AbsPtrType toLValue();
      AbsPtrType toRValue();

      void accept(TypeVisitor* v);
  };


  /* Typ funkcyjny.
   *
   * Posiada listę argumentów oraz identyfikatorów oraz zna typ wynikowy. */
  class FunType : public InvalidType {
    private:
      std::string str;

    public:
      typedef boost::shared_ptr<FunType> PtrType;

      Fun *body;
      std::string name;
      std::vector<AbsPtrType> args;
      std::vector<std::string> idents;
      AbsPtrType ret_type;
      ClassType::PtrType cls;

      FunType(std::string name);
      TypeType getType();
      bool isA(AbstractType &el);
      std::string& toString();

      void accept(TypeVisitor* v);
  };


  /* Typ tablicowy
   *
   * Zna typ elementów. */
  class ArrayType : public InvalidType {
    private:
      std::string str;

    public:
      typedef boost::shared_ptr<ArrayType> PtrType;

      int length;
      bool undefined;
      AbsPtrType elementsType;

      ArrayType(AbsPtrType ptr);
      ArrayType(AbsPtrType ptr, int l);
      TypeType getType();
      bool isA(AbstractType& el);
      std::string& toString();

      AbsPtrType toLValue();
      AbsPtrType toRValue();

      void accept(TypeVisitor* v);
  };


  class TypeVisitor {
    public:
      virtual void visitInt(IntType* t);
      virtual void visitVoid(VoidType* t);
      virtual void visitString(StringType* t);
      virtual void visitInvalid(InvalidType* t);
      virtual void visitBool(BoolType* t);
      virtual void visitFun(FunType* t);
      virtual void visitArray(ArrayType* t);
      virtual void visitClass(ClassType* t);
      virtual void visitNull(NullType* t);
  };
}

#endif
