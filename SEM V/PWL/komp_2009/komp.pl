%wersja ze stałymi stałymi


%para(X, Y)	<=> X jest nawiasem otwierajacym, a Y go zamyka
para(a, d).
para(b, c).


%dodaj(L, X, S) <=> 	L - lista par (nawias-otwierajacy, wartosc)
%		    	X - nawias
%			S - lista par (nawias-otwierajacy, wartosc) 
%			    uwzgledniajacy X
dodaj([], Y, [[Y, 1]]) :- para(Y, _).
dodaj([ [X, C] | L], X, [S | L]) :- para(X, _), C2 is C + 1, S = [X, C2], !. % obciecie redundantnych rozwiazan
dodaj([ [X, C] | L], Y, [S | L]) :- para(X, Y), C2 is C - 1, S = [X, C2], !. % obciecie redundantnych rozwiazan
dodaj([ [X, C] | L], Z, [ [X, C] | S ]) :- dodaj(L, Z, S). % prawdziwy dla Prologowej funkcji wyboru (formalnie powinienem użyć negacji).

%nieujemne(L) <=>	L - lista par (nawias, wartosc), gdzie kazda wartosc nieujemna
nieujemne([]).
nieujemne([ [_, C] | L ]) :- C >= 0, nieujemne(L).

%zerowe(L) <=>	L - lista par (nawias, wartosc), gdzie wartosc = 0
zerowe([]).
zerowe([ [_, 0] | L]) :- zerowe(L).

%wn(L) <=>	L jest poprawnym wyrazeniem nawiasowym zdefiniowanym przez predykat para(X, Y)
wn(L) :- wn(L, []).
wn([], X) :- zerowe(X).
wn([ X | L ], A) :- dodaj(A, X, A2), nieujemne(A2), wn(L, A2).

