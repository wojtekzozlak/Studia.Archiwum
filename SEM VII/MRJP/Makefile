SHELL := /bin/bash

CC = g++
CCFLAGS = -g -I src -rdynamic
FLEX = flex
BISON = bison
LATEX = latex
DVIPS = dvips


DEPS = bin/syntaxCheck/SyntaxChecker.o bin/Parser.o bin/Absyn.o bin/Lexer.o bin/Printer.o bin/latc.o bin/parser/VisitorSkeleton.o bin/core/Types.o bin/syntaxCheck/Eval.o bin/core/Environment.o bin/x86/Assembler.o bin/x86/Eval.o bin/x86/Operations.o


all: latc-x86


clean:
	rm bin/* -R -f
	rm latc-jvm -f
	rm latc-parser -f
	rm latc -f
	rm latc-x86 -f

latc-parser: $(DEPS)
	${CC} ${CCFLAGS} $^ -o $@


latc-x86: DEFS = -DX86_COMPILER
latc-x86: $(DEPS) bin/x86/runtime.o
	$(CC) ${CCFLAGS} $(DEPS) -o $@
	cp latc-x86.sh latc

latc-jvm: DEFS = -DJVM_COMPILER
latc-jvm: $(DEPS)
	${CC} ${CCFLAGS} $^ -o $@
	cp latc-jvm.sh latc
	

bin/Absyn.o: src/parser/Absyn.C src/parser/Absyn.H
	${CC} ${CCFLAGS} -c src/parser/Absyn.C -o $@

src/parser/Lexer.C: src/grammar/latte.l
	${FLEX} -o$@ src/grammar/latte.l

src/parser/Parser.C: src/grammar/latte.y
	${BISON} src/grammar/latte.y -o $@

bin/Lexer.o: src/parser/Lexer.C src/parser/Parser.H
	${CC} ${CCFLAGS} -c src/parser/Lexer.C -o $@

bin/Parser.o: src/parser/Parser.H src/parser/Parser.C src/parser/Absyn.H
	${CC} ${CCFLAGS} -c src/parser/Parser.C -DYYERROR_VERBOSE -o $@

bin/Printer.o: src/parser/Printer.C src/parser/Printer.H src/parser/Absyn.H
	${CC} ${CCFLAGS} -c src/parser/Printer.C -o $@

bin/x86/runtime.o: src/x86/runtime.s
	${CC} ${CCFLAGS} -m32 -O0 -c src/x86/runtime.s -o bin/x86/runtime.o

bin/%.o: src/%.cpp src/%.h
	mkdir -p `dirname $@`
	${CC} ${CCFLAGS} ${DEFS} -c $< -o $@
