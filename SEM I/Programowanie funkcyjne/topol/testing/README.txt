- Uwaga! 
Ca³oœæ zak³ada, ¿e topol opiera siê tylko i wy³¹cznie na module PMap z zadania drugiego. Jeœli Twój program korzysta z czegoœ innego, popraw mój obleœny makefile.


- Korzystamy
1. wrzucamy topol.ml do g³ównego katalogu
2. make
  tworzy potrzebne sobie rzeczy i plik wykonywalny 'test'
3. ./test
  odpala test na testach z katalogu 'in', wywala output do 'out' a potem grepem szuka b³êdów


- Generowanie testów
1. make test
  tworzy 10 nowych testów w katalogu 'in' (nadpisuj¹c stare!) ich kszta³tem steruje 'source/gen.cpp'
  
- Czyszczenie
1. make clean
  usuwa skompilowane pliki umo¿liwiaj¹c ponowne u¿ycie 'make'