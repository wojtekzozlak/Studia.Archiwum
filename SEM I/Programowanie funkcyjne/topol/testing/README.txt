- Uwaga! 
Ca�o�� zak�ada, �e topol opiera si� tylko i wy��cznie na module PMap z zadania drugiego. Je�li Tw�j program korzysta z czego� innego, popraw m�j oble�ny makefile.


- Korzystamy
1. wrzucamy topol.ml do g��wnego katalogu
2. make
  tworzy potrzebne sobie rzeczy i plik wykonywalny 'test'
3. ./test
  odpala test na testach z katalogu 'in', wywala output do 'out' a potem grepem szuka b��d�w


- Generowanie test�w
1. make test
  tworzy 10 nowych test�w w katalogu 'in' (nadpisuj�c stare!) ich kszta�tem steruje 'source/gen.cpp'
  
- Czyszczenie
1. make clean
  usuwa skompilowane pliki umo�liwiaj�c ponowne u�ycie 'make'