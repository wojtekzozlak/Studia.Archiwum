para([ [X, Y] | _L ], X, Y).
para([_Z | L], X, Y) :- para(L, X, Y).

otwiera([ [X] | _L], X).
otwiera([ [X, _Y] | _L], X).
otwiera([_Z | L], X) :- otwiera(L, X).

istnieje(D, X) :- otwiera(D, X).
istnieje(D, X) :- para(D, _, X).

wzbogac(D, X, D) :- istnieje(D, X).
wzbogac([], X, [ [X] ]).
wzbogac([ [X] | L], Y, [ [X, Y] | L]) :- istnieje([ [X] | L], Y) -> false ; true.
wzbogac([ X | L], Z, [ X | D]) :- istnieje([X | L], Z) -> false ;  wzbogac(L, Z, D).

dodaj([], X, D, [ [X, 1] ]) :- otwiera(D, X).
dodaj([ [X, C] | L ], X, D, [ [X, C2] | L ]) :- otwiera(D, X), C2 is C + 1.
dodaj([ [X, C] | L ], Y, D, [ [X, C2] | L ]) :- para(D, X, Y), C > 0, C2 is C - 1.
dodaj([ [X, C] | L ], Y, D, [ [X, C] | S ]) :- X \= Y, dodaj(L, Y, D, S).


zerowe([]).
zerowe([ [_X, 0] | L ]) :- zerowe(L).



wn(L) :- wn(L, [], []).
wn([], _D, S) :- zerowe(S).
wn([X | L], D, S) :- wzbogac(D, X, D2), dodaj(S, X, D2, S2), wn(L, D2, S2).
