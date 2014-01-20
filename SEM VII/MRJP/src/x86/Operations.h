/****************************************************************************
 * Operations.h
 *
 * Zbiór pomocniczych funkcji generujących instrukcje asemblera. Mogą
 * modyfikować rejestry %edx, %ecx.
 *
 * Funkcje zawierające "reg" w nazwie oczekują, że co najmniej jeden
 * z argumentów jest rejestrem. W zamian - są szybsze i zwykle nie modyfikują
 * żadnych innych rejestrów.
 ****************************************************************************/
#ifndef _X86_OPERATIONS_
#define _X86_OPERATIONS_

#include <ostream>
#include "Assembler.h"

namespace x86 {
namespace Operations {

using std::string;

void movl(std::ostream& stream, const string& src, const string& dest);

void movl(std::ostream& stream, int i, const string& dest);

void movregl(std::ostream& stream, const string& src, const string& dest);

void movregl(std::ostream& stream, int i, const string& dest);

void movb(std::ostream& stream, const string& src, const string& dest);

void movb(std::ostream& stream, int i, const string& dest);

void movregb(std::ostream& stream, const string& src, const string& dest);

void movlabl(std::ostream& stream, const string& label, const string& dest);

void cmpl(std::ostream& stream, const string& v1, const string& v2);

void cmpb(std::ostream& stream, const string& v1, const string& v2);

void cmp(std::ostream& stream, int t_size, const string& v1, const string& v2);

void jmp(std::ostream& stream, const string& label);

void incl(std::ostream& stream, const string& val);

void decl(std::ostream& stream, const string& val);

void addl(std::ostream& stream, int i, const string& dest);

void addregl(std::ostream& stream, const string& a, const string& b);

void subl(std::ostream& stream, int i, const string& dest);

void subregl(std::ostream& stream, const string& a, const string& b);

void imul(std::ostream& stream, const string& a, const string& b);

void idivl(std::ostream& stream, const string& a);

void negl(std::ostream& stream, const string& a);

void sal(std::ostream& stream, int i, const string& what);

void sarl(std::ostream& stream, int i, const string& what);

void lab(std::ostream& stream, const string& lab);

void cndjmp(std::ostream& stream, const string& op, const string& lab);

void movsp(std::ostream& stream, int bp_offset);

void call(std::ostream& stream, const string& fun);

void rel(const string& op, ExprAssembler* ass, int size);

}
}


#endif
