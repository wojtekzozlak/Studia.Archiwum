Skrypty napisane w perlu.
Odpalać w folderze z rational.h i rational.cc (lepiej sobie zrobić backup).

Np.
chmod +x test3
./test3

albo

perl ./test3

Generują małe fragmenty kodu, kompilują z rational i zapisują wynik (ewentualnie błąd kompilacji).

test3 to testy tylko operatorów arytmetycznych (w tym wypisania na strumień).
test2 to testy tylko operatorów porównań i przypisania (w tym wypisania na strumień).
test to testy losowe wszystkich operatorów i jakieś dziwne wpisane z palca.

Testy mogą chodzić dosyć długo, bo jest ich dosyć sporo (wygenerowanych). Niektóre może nie są zbyt sensowne.

Zostaną wygenerowane pliki: out<liczba> oraz gcc_out<liczba>, gdzie <liczba> to numer testu.
Na stronie są też wygenerowane outy (oby wzorcowe), najlepiej porównać je diffem.

Np. diff out3 <ścieżka_do_out3_z_out3.tar.lzma>

gcc_out<liczba> - to wyjście błędów z gcc (tego nie trzeba porównywać)
out<liczba> - to wyjście z kodem testów oraz wynikiem odpalenia (w tym błędów kompilacji)

Raz na jakiś czas wyskakuje: Floating point exception. To oznacza, że wygenerował się test
gdzie longa dzielimy przez zero. Nie trzeba się martwić tym. Pole Output w out jest wtedy puste w danym teście.

Swoją drogą te skrypty nie są zbyt genialnie napisane, więc pewnie można je poprawić.

Jakby były jakieś wątpliwości (błędy w out na stronie) albo propozycje wrednych testów, to śmiało pisać.




* (dla chętnych): Można utworzyć folder, którego zawartość będzie trzymana wyłącznie w ramie,
				  a następnie wszystko tam wrzucić: testy i rationale (cc i h). (wymaga roota)
				  A outy rozpakować gdzieś indziej. Powinno to być bardziej oszczędne dla dysku (i może szybsze).
				  Np.
					 mkdir <nazwa_folderu>
					 mount -t tmpfs -o size=100M,mode=0755 tmpfs <nazwa_folderu>
				  Ewentulanie potem to chownować i chgrpować dla wygody (wymaga roota).
					 chown <user> <nazwa_folderu>
					 chgrp <user> <nazwa_folderu>
				  Usunąć to można:
					 umount <nazwa_folderu>
					 rmdir <nazwa_folderu>





Wojciech Sirko (wsirko  _at_  gmail.com)
