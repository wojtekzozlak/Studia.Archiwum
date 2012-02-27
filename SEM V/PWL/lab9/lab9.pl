%drzewo(D) :- D jest drzewem binarnym
drzewo(nil).
drzewo(tree(X, _W, Y)) :- drzewo(X), drzewo(Y).

%ileW(D, K) zlicza wierzcholki w drzewie
ileW(D, K) :- ileW(D, 0, K).
ileW(nil, S, S).
ileW(tree(X, _W, Y), S, K3) :- ileW(X, S, K1), ileW(Y, K1, K2), K3 is K2 + 1.

%insertBST(D, E, N)  <=> N powstaje z D przez dodanie E
insertBST(nil, E, tree(nil, E, nil)) :- !.
% insertBST(tree(X, W, Y), W, tree(X, W, Y)) :- true, !.    wersja z jednokrotnymi i sukcesem
insertBST(tree(X, W, Y), E, tree(X2, W, Y)) :- W > E, !, insertBST(X, E, X2).
insertBST(tree(X, W, Y), E, tree(X, W, Y2)) :- W =< E, insertBST(Y, E, Y2). % moge usunac = i bedzie jednokrotnie + fail 


%createBST(L, D) tworzy drzewo BST z listy L
createBST(L, D) :- createBST(L, nil, D).
createBST([], A, A).
createBST([X | L], A, D) :- insertBST(A, X, A2), createBST(L, A2, D).

%wypiszBST(D) wypisuje w porzadku infiksowym
wypiszBST(nil).
wypiszBST(tree(X, W, Y)) :- wypiszBST(X), write(W), wypiszBST(Y).

%wypiszBST(D, L) wypisuje w porzadku infiksowym na liste
wypiszBST(D, L) :- wypiszBST(D, [], L).
wypiszBST(nil, A, A).
wypiszBST(tree(X, W, Y), A, L2) :- wypiszBST(Y, A, L1), wypiszBST(X, [W | L1], L2).

%sortBST(L, S) S jest wynikiem sortowanie L uzywajac BST
sortBST(L, S2) :- createBST(L, S1), wypiszBST(S1, S2).

%wypiszPoziom(D, N, L) wypisuje wierzcholki na poziomie N do listy L
wypiszPoziom(D, N, L) :- wypiszPoziom(D, N, [], L).
wypiszPoziom(nil, _N, L, L).
wypiszPoziom(tree(_X, W, _Y), 1, A, [W | A]).
wypiszPoziom(tree(X, _W, Y), N, A, A2) :- N > 0, N2 is N - 1, !, wypiszPoziom(Y, N2, A, A1), wypiszPoziom(X, N2, A1, A2).

%wszerz(D, L)  przejscie BFS
wszerz(D, L) :- wszerz(D, 1, [], L).
wszerz(D, N, L) :- wypiszPoziom(D, N, L, L).
wszerz(D, N, L) :- N2 is N + 1, wypiszPoziom(D, N2, L, L2), wypiszPoziom(D, N, L2, L3)
