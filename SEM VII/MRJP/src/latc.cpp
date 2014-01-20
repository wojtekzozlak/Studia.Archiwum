#include <iostream>
#include <stdio.h>
#include "syntaxCheck/SyntaxChecker.h"
#include "core/Environment.h"
#include "parser/Parser.H"
#include "parser/Printer.H"
#include "parser/Absyn.H"
#include "x86/Assembler.h"
#include "core/Types.h"


#include <execinfo.h>
#include <signal.h>

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, 2);
  exit(1);
}



int main(int argc, char ** argv)
{
  signal(SIGSEGV, handler);
  signal(SIGABRT, handler);

  FILE *input;
  if (argc > 1)
  {
    input = fopen(argv[1], "r");
    if (!input)
    {
      fprintf(stderr, "Error opening input file.\n");
      exit(1);
    }
  }
  else input = stdin;

  using namespace SyntaxCheck;
  /* The default entry point is used. For other options see Parser.H */
  LProgram *program_tree = pLProgram(input);
  if (program_tree)
  {
    SyntaxCheck::SyntaxChecker synChecker;
    program_tree->accept(&synChecker);

    if (SyntaxCheck::parsing_successful) {
      std::cerr << "OK" << std::endl;
    } else {
      return 1;
    }


  #ifdef X86_COMPILER


    x86::Assembler x86Ass(&synChecker.env);
    x86Ass.assembly();
  #endif


    return 0;
  }
  return 1;
}
