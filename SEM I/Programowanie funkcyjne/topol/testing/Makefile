start: bin/test
	cp source/test ./

tests: bin/gen
	./bin/gen > ./in/00.in
	./bin/gen > ./in/01.in
	./bin/gen > ./in/02.in
	./bin/gen > ./in/03.in
	./bin/gen > ./in/04.in
	./bin/gen > ./in/05.in
	./bin/gen > ./in/06.in
	./bin/gen > ./in/07.in
	./bin/gen > ./in/08.in
	./bin/gen > ./in/09.in
	./bin/gen > ./in/10.in

bin/gen:
	g++ source/gen.cpp -o bin/gen

bin/pMap.cmi:
	ocamlc -c source/pMap.mli 
	mv source/pMap.cmi bin/

bin/pMap.cmo: bin/pMap.cmi
	cp source/pMap.ml bin/
	ocamlc -c bin/pMap.ml
	rm bin/pMap.ml

bin/topol.cmi: bin/pMap.cmo
	ocamlc -c source/topol.mli 
	mv source/topol.cmi bin/

bin/topol.cmo: bin/topol.cmi
	cp topol.ml bin/
	cd bin && ocamlc -c topol.ml
	rm bin/topol.ml

bin/test.cmo: bin/topol.cmo
	cp source/test.ml bin/
	cd bin && ocamlc -c test.ml
	rm bin/test.ml

bin/test: bin/pMap.cmo bin/topol.cmo bin/test.cmo
	cd bin && ocamlc -o test pMap.cmo topol.cmo test.cmo

clean:
	rm bin/* -f
	rm test
