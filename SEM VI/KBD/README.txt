Instrukcja obslugi. Uwaga - proponuje ogladac programy w kolejnosci, w ktorej sa wymienione tutaj:
- generic_benchmark.*
- benchmark_sample.cc
- benchmark_lookup.cc

1) Otworzyc generic_benchmark.h i przeczytac instrukcje. Mozna w celu zrozumienia spojrzec na implementacje w benchmark.cc, trudna nie jest. Spojrzec na przykladowe uzycie w benchmark_sample.cc.
2) Uruchomic:
g++ -O0 -Wall generic_benchmark.cc benchmark_sample.cc -o sample
./sample
3) Spojrzec na to, ile daje optymalizacja:
g++ -O6 -Wall generic_benchmark.cc benchmark_sample.cc -o sample
(potem powiemy, skad sie bierze to przyspieszenie, choc pomysly mozna zglaszac uze teraz).

4) Pierwszy eksperyment - lookup. Mamy tablice table_, ktora zawiera jakies losowe liczby, oraz tablice input_, ktora zawiera permutacje liczb od 1 do N. Chcemy przepisac table_ do result_, stosujac zadana permutacje - czyli wpisac w pole result_[i] liczbe table_[input_[i]].
To wlasnie dzieje sie w benchmark_lookup.cc, ale proponuje napisac to samemu, zeby pocwiczyc uzywanie klasy Benchmark - tablice mozna alokowac dynamicznie lub statycznie, nie robi roznicy, w funkcji Run powinna byc prosta petla i przypisanie jakiejs losowej liczby na dummy_.

Pytanie, jak to sie zachowuje ze zmieniajacym sie N. Z checia poslucham odpowiedzi, i strzalow, zanim zapuscimy program, benchmark_lookup.cc sprawdza to (najlepiej puszczac z -O6).

5) Nastepny w kolejce jest benchmark_reach, ktory jest mniej wiecej o tym samym. Tego nie piszemy, tylko czytamy.

6) Kolejna zagadka. Filtrowanie (czesty problem w bazach danych).

Mamy tablice liczb. Chcemy znalezc indeksy tych, ktore spelniaja pewien warunek (powiedzmy, ze liczby sa od 1 do 100, i chcemy znalezc te, ktore sa wieksze od S, gdzie S jest jakas liczba, zapewne kolo 50). Konkretniej - mamy tablice wejsciowa T, oraz tablice wyjsciowa O, i chcemy, zeby w O znalazly sie takie i, ze T[i] < S. Po kolei, i bez spacji, czyli dla T = { 3, 1, 4, 1, 5, 9 }, S = 4 chcemy w O miec { 0, 1, 3 }, a dalej niewypelnione. Funkcja ma zwracac rozmiar tablicy O (tablica O jest od razu zaalokowana i jest wystarczajaco duza).

Prosze napisac i zmierzyc zachowanie dla roznych wartosci S.

A potem obejrzec benchmark_find.cc i zobaczyc, co tam sie dzieje.

7) W zaleznosci od tego, ile czasu nam zostalo, jeszcze sprawdzmy to, ile kosztuje wywolanie funkcji, i ile kosztuje wywolanie wirtualne. Napisac samemu lub odpalic benchmark_functions.cc
