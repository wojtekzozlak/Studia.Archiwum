dodaj(liczba(X, Y), liczba(Y, Z), liczba(X, Z)).


% dlugosc(L, K) <=> K = długość listy L
dlugosc([], 0).
dlugosc([_X | L], K) :- dlugosc(L, K2), K is K2 + 1.

% suma(L, S) <=> S = suma eltow listy L
suma(L, S) :- suma(L, 0, S).
suma([], X, X).
suma([X | L], A, S) :- A1 is X + A, suma(L, A1, S).

% fib(K, N) <=> N jest K-ta liczba Fibbonaciego
%
% nie działa gdy K jest zmienną :/
fib(K, N) :- fib(K, 0, 1, N).
fib(0, X, Z, X) :- integer(X), integer(Z).
fib(K, F1, F2, N) :- integer(F1), integer(F2),
                     K2 is (K - 1),
                     F3 is (F1 + F2),
		     ( (integer(N), F1 > N ) -> false ; fib(K2, F2, F3, N) ).

% fib2(K, F1, F2, NR, N)
fib2(K, N) :- fib2(K, 0, 0, 1, N).
fib2(K, K, X, _Z, X).
fib2(K, NR, F1, F2, N) :- (integer(N), F1 > N) -> false ; 
                                F3 is F1 + F2, NR2 is NR + 1, 
                                fib2(K, NR2, F2, F3, N).

% poziom(listalist, K, listaK) <=> 	listalist - zagniezdzona lista o eltach calkowitych
%					K - liczba naturalna
%					listaK - lista eltow z poziomu K, w oryginalnym porzadku
poziom(LL, K, LK) :- poziom(LL, K, [], LK).
poziom([], _, X, X).
poziom(X, 0, A, [X | A]) :- integer(X).
poziom([X | L], K, A, LK) :- poziom(L, K, A, A2), K2 is K - 1, poziom(X, K2, A2, LK).
