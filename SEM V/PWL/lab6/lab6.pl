% dobre(n, p) <=> n = liczba nat. (0, s/1), p-zmienna
%			p = podstawienie t. że
%			p^n = Eps,	Forall i <= k < n   p^k != Eps
dobre(0, []).
dobre(s(0), []).
dobre(s(s(N)), [ p(Y, Z) | L ]) :- dobre(N, L, Z, Y).

dobre(0, [ p(X, Y) ], X, Y).
dobre(s(N), [ p(Y, Z) | L ], Y, B) :- dobre(N, L, Z, B).

%fp(L, F) <=> l = lista {b, c}, F = posortowane
fp(L, X) :- przesiej(L, B, C), append(B, C, X).
przesiej([], [], []).
przesiej([b | L], [b | B], C) :- przesiej(L, B, C).
przesiej([c | L], B, [c | C]) :- przesiej(L, B, C).


fp1(L, X) :- fp1(L, [], [], X).
fp1([], B, C, F) :- append(B, C, F).
fp1([b | L], B, C, F) :- fp1(L, [b | B], C, F).
fp1([c | L], B, C, F) :- fp1(L, B, [c | C], F).

% qsort(l, s)	<=>	s = wynik sortowania l

qsort([], []).
qsort([X | L], S) :- partition(L, X, LM, LW), qsort(LM, LMS), qsort(LW, LWS), append(LMS, [X | LWS], S).
