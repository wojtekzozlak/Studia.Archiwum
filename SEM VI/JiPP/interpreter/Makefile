all:
	cd src && bnfc -m -haskell -p Grammar Grammar.cf
	cd src && make
	cd src && ghc --make Main.hs -o ../interpreter 

clean:
	cd src && make clean
	rm src/Grammar -R
	rm src/*.hi src/*.o
