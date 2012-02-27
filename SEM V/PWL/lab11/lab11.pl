sasiedzi(a, [b, c, d]).
sasiedzi(b, [d]).
sasiedzi(c, [d]).
sasiedzi(d, []).

odlegle(W1, W2, Odleglosci) :-
	sasiedzi(W1, LW1), sasiedzi(W2, _), odlegle(LW1, [], W2, 1, Odleglosci).

odlegle([], [], _, _, []) :- !.

odlegle([], A, W2, D, Odleglosci) :-
	D2 is D + 1,
	odlegle(A, [], W2, D2, Odleglosci).

odlegle([W2 | L], A, W2, D, [D | Odleglosci]) :- !,
	odlegle(L, A, W2, D, Odleglosci).

odlegle([X | L], A, W2, D, Odleglosci) :-
	X \= W2,
	sasiedzi(X, Sasiedzi),
	append(A, Sasiedzi, A2),
	odlegle(L, A2, W2, D, Odleglosci).





%dodaj(E, L) - dodanie E do listy otwartej L o ile E nie ma w L
dodaj(E, [E | _]) :- !.
dodaj(E, [_ | L]) :-
	dodaj(E, L).

%insertBST(E, Tree) - wstawienie do otwartego drzewa BST
insertBST(E, X) :-
	var(X), !,
	X = tree(_, E, _).

insertBST(E, tree(L, W, _)) :-
	E =< W, !,
	insertBST(E, L).

insertBST(E, tree(_, W, P)) :-
	W < E,
       	insertBST(E, P).

%insertBST2(E, Tree) - wstawienie jednokrotne
insertBST2(E, tree(_, E, _)) :- !.

insertBST2(E, tree(L, W, _)) :-
	E < W, !,
	insertBST(E, L).

insertBST2(E, tree(_, W, P)) :-
	E > W,
       	insertBST(E, P).


%closeBST(L) - zamyka drzewo BST
closeBST(nil) :- !.

closeBST(tree(L, _, P)) :-
	closeBST(L),
	closeBST(P).



