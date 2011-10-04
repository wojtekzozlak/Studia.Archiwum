anagramy: pMap.cmo strAnagramHash.cmo klasy.cmo anagramy.cmo
	ocamlc -o $@ $^

%.cmo: %.ml
	ocamlc -c $<

%.cmi: %.mli
	ocamlc -c $<

clean:
	rm *.cmi *.cmo

depend:
	ocamldep *.ml *.mli > .depend

include .depend
