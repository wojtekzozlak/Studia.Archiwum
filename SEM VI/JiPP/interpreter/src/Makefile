all:
	happy -gca Grammar/ParGrammar.y
	alex -g Grammar/LexGrammar.x
	(cd Grammar/; latex DocGrammar.tex; dvips DocGrammar.dvi -o DocGrammar.ps)
	ghc --make Grammar/TestGrammar.hs -o Grammar/TestGrammar
clean:
	-rm -f Grammar/*.log Grammar/*.aux Grammar/*.hi Grammar/*.o Grammar/*.dvi
	-rm -f Grammar/DocGrammar.ps
distclean: clean
	-rm -f Grammar/DocGrammar.* Grammar/LexGrammar.* Grammar/ParGrammar.* Grammar/LayoutGrammar.* Grammar/SkelGrammar.* Grammar/PrintGrammar.* Grammar/TestGrammar.* Grammar/AbsGrammar.* Grammar/TestGrammar Grammar/ErrM.* Grammar/SharedString.* Grammar/Grammar.dtd Grammar/XMLGrammar.* Makefile*
	-rmdir -p Grammar/
