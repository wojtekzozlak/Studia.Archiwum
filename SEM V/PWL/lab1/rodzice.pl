% matka(Matka, Dziecko)
matka(X, Y) :- dziecko(Y, X, _).

% ojciec(Ojciec, Dziecko)
ojciec(X, Y) :- dziecko(Y, _, X).

% rodzic(Rodzic, Dziecko)
rodzic(X, Y) :- matka(X, Y).
rodzic(X, Y) :- ojciec(X, Y).

% babcia(Babcia, Wnuczka)
babcia(X, Y) :- matka(X, Z), rodzic(Z, Y).

% wnuk(Wnuk, Dziadek)
wnuk(X, Y) :- rodzic(Y, Z), rodzic(Z, X).

% przodek(Przodek, Potomek)
przodek(Prz, Po) :- rodzic(Prz, Po).
przodek(Prz, Po) :- rodzic(Prz, Ktos), przodek(Ktos, Po).

% potomek(Potomek, Przodek)
potomek(Po, Prz) :- przodek(Prz, Po).
