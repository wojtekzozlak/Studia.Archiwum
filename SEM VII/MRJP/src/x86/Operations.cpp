#include <assert.h>
#include <map>
#include <utility>
#include "Operations.h"

namespace x86 {
namespace Operations {

using std::make_pair;
using std::string;

typedef std::map< std::pair<string, int>, string> RegMap;


RegMap init_reg_map() {
  RegMap map;
  map.insert(make_pair(make_pair("%ecx", 4), "%ecx"));
  map.insert(make_pair(make_pair("%ecx", 1), "%cl"));

  return map;
};
RegMap reg_map = init_reg_map();


string reg(const string name, int size) {
  RegMap::iterator it = reg_map.find(make_pair(name, size));
  assert(it != reg_map.end());
  return it->second;
};


void movl(std::ostream& stream, const string& src, const string& dest) {
  stream << "	movl	" << src << ", %ecx" << std::endl;
  stream << "	movl	%ecx, " << dest << std::endl;
};


void movl(std::ostream& stream, int i, const string& dest) {
  stream << "	movl	$" << i << ", " << dest << std::endl;
};


void movregl(std::ostream& stream, const string& src, const string& dest) {
  stream << "	movl	" << src << ", " << dest << std::endl;
};


void movregl(std::ostream& stream, int i, const string& dest) {
  stream << "	movl	$" << i << ", " << dest << std::endl;
};


void movb(std::ostream& stream, const string& src, const string& dest) {
  stream << "	movb	" << src << ", %cl" << std::endl;
  stream << "	movb	%cl, " << dest << std::endl;
};


void movb(std::ostream& stream, int i, const string& dest) {
  stream << "	movb	$" << i << ", " << dest << std::endl;
};


void movregb(std::ostream& stream, const string& src, const string& dest) {
  stream << "	movb	" << src << ", " << dest << std::endl;
};


void movlabl(std::ostream& stream, const string& label, const string& dest) {
  stream << "	movl	$" << label << ", " << dest << std::endl;
};


void cmpl(std::ostream& stream, const string& v1, const string& v2) {
  stream << "	movl	" << v2 << ", %ecx" << std::endl;
  stream << "	cmp	%ecx, " << v1 << std::endl;
};

void cmpb(std::ostream& stream, const string& v1, const string& v2) {
  stream << "	movb	" << v2 << ", " << reg("%ecx", 1) << std::endl;
  stream << "	cmp	%cl, " << v1 << std::endl;
};

void cmp(std::ostream& stream, int t_size, const string& v1, const string& v2) {
  if (t_size == 1)
    cmpb(stream, v1, v2);
  else if (t_size == 4)
    cmpl(stream, v1, v2);
  else assert(false);
};

void jmp(std::ostream& stream, const string& label) {
  stream << "	jmp	" << label << std::endl;
};

void incl(std::ostream& stream, const string& val) {
  stream << "	incl	" << val << std::endl;
};


void decl(std::ostream& stream, const string& val) {
  stream << "	decl	" << val << std::endl;
};


void addl(std::ostream& stream, int i, const string& dest) {
  stream << "	addl	$" << i << ", " << dest << std::endl;
};


void addregl(std::ostream& stream, const string& a, const string& b) {
  stream << "	addl	" << a << ", " << b << std::endl; 
};


void subl(std::ostream& stream, int i, const string& dest) {
  stream << "	subl	$" << i << ", " << dest << std::endl;
};


void subregl(std::ostream& stream, const string& a, const string& b) {
  stream << "	subl	" << a << ", " << b << std::endl;
};


void imul(std::ostream& stream, const string& a, const string& b) {
  stream << "	imul	" << a << ", " << b << std::endl;
};


void idivl(std::ostream& stream, const string& a) {
  stream << "	idivl	" << a << std::endl;
};


void negl(std::ostream& stream, const string& a) {
  stream << "	negl	" << a << std::endl;
};


void sal(std::ostream& stream, int i, const string& what) {
  stream << "	sal	$" << i << ", " << what << std::endl;
};


void sarl(std::ostream& stream, int i, const string& what) {
  stream << "	sarl	$" << i << ", " << what << std::endl;
};


void lab(std::ostream& stream, const string& lab) {
  stream << lab << ":" << std::endl;
};


void cndjmp(std::ostream& stream, const string& op, const string& lab) {
  stream << "	" << op << "	" << lab << std::endl;
};


void movsp(std::ostream& stream, int bp_offset) {
  stream << "	movl	%ebp, %esp" << std::endl;
  stream << "	subl	$" << -bp_offset << ", %esp" << std::endl;
};


void call(std::ostream& stream, const string& fun) {
  stream << "	call	" << fun << std::endl;
};


void rel(const string& op, ExprAssembler* ass, int size) {
  std::ostream& out = ass->ass_env->stream;

  bool crystalize = ass->if_true.compare(string()) == 0;

  cmp(ass->ass_env->stream, size, ass->arg1, ass->arg2);

  if (crystalize) {
    int lab = ass->scope->nextLab();
    out << "	" << op << "	" << ass->ass_env->fun_name << "_EQU_T" <<
        lab << std::endl;
    out << "	movb	$0, " << ass->dest << std::endl;
    out << "	jmp	" << ass->ass_env->fun_name << "_EQU_END" <<
        lab << std::endl;
    out << ass->ass_env->fun_name << "_EQU_T" << lab << ":" <<
        std::endl;
    out << "	movb	$1, " << ass->dest << std::endl;
    out << ass->ass_env->fun_name << "_EQU_END" << lab << ":" << std::endl;
  } else {
    out << "	" << op << "	" << ass->if_true << std::endl;
    out << "	jmp	" << ass->if_false << std::endl;
  }
};



}
}
