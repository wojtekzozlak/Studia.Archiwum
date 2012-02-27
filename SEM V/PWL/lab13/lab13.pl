:- op(500, xfx, [--, ++]).

%norm(W, PN) - normalizuje do wyrazenia z prawym nawiasowaniem

norm(A, H) :-
    norm(A, H ++ T, Y),
    T = Y.
    
norm(A, X ++ X, A) :-
    atomic(A), !.

norm(A + B, AH ++ BT, BB) :-
    norm(A, AH ++ AT, AA),
    norm(B, BH ++ BT, BB),
    AT = (AA + BH).

%wersja hakierska
norm2(W, PN) :-
    normSR(W, PN ++ 0).

normSR(A, (A + Z) ++ Z) :-
    atomic(A),
    var(Z), !.

normSR(A, A ++ 0) :-
    atomic(A), !.

normSR(A + B, PA ++ KB) :-
    normSR(A, PA ++ KA),
    normSR(B, KA ++ KB).
