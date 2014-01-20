/****************************************************************************
 * Errors.h
 *
 * Klasa opisująca typowe błędy dla operacji arytmetycznych i logicznych.
 ****************************************************************************/
#ifndef _SYNTAXCHECK_ERRORS_
#define _SYNTAXCHECK_ERRORS_

#include "parser/Absyn.H"
#include "parser/VisitorSkeleton.h"

namespace SyntaxCheck {

  class GeneralError : public VisitorSkeleton {
    private:
      std::string* msg;
      GeneralError() : msg(NULL) {} ;

    public:
      static GeneralError& obj() {
        static GeneralError gen_err;
        return gen_err;
      }

      std::string& getMsg(Visitable *op) {
        op->accept(this);
        assert(msg != NULL);
        return *msg;
      }

      void visitNot(Not *p) {
        static std::string str("Logical negation of type");
        msg = &str;
      }

      void visitNeg(Neg *p) {
        static std::string str("Arithmetical negation of type");
        msg = &str;
      }

      void visitPlus(Plus *p) {
        static std::string str("Addition of types");
        msg = &str;
      }

      void visitMinus(Minus *p) {
        static std::string str("Subtraction of types");
        msg = &str;
      }

      void visitDiv(Div *p) {
        static std::string str("Division of types");
        msg = &str;
      }

      void visitTimes(Times *p) {
        static std::string str("Multiplication of types");
        msg = &str;
      }

      void visitMod(Mod *p) {
        static std::string str("Modulo of types");
        msg = &str;
      }

      void visitLTH(LTH *p) {
        static std::string str("Comparison (<) of types");
        msg = &str;
      }

      void visitLE(LE *p) {
        static std::string str("Comparison (<=) of types");
        msg = &str;
      }

      void visitGTH(GTH *p) {
        static std::string str("Comparison (>) of types");
        msg = &str;
      }

      void visitGE(GE *p) {
        static std::string str("Comparison (>=) of types");
        msg = &str;
      }

      void visitEQU(EQU *p) {
        static std::string str("Comparison (==) of types");
        msg = &str;
      }

      void visitNE(NE *p) {
        static std::string str("Comparison (!=) of types");
        msg = &str;
      }

      void visitEAnd(EAnd *p) {
        static std::string str("Disjunction of types");
        msg = &str;
      }

      void visitEOr(EOr *p) {
        static std::string str("Conjunction of types");
        msg = &str;
      }
  };

}


#endif
