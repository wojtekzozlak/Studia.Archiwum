%flatten(X, Y) <-> Y jest "spłaszczoną" listą X
flatten(X, Z) :- flatten(X, [], Z).
flatten([], A, A).
flatten(X, A, [X | A]) :- integer(X).
flatten([X | L], A, A3) :- flatten(X, A2, A3), flatten(L, A, A2).


%slowo(S) <=> S = a^n b^n
%dla n >= 0
slowo1(S) :- slowo1(S, []).
slowo1([a | L], A) :- slowo1(L, [b | A]).
slowo1(S, S).

%dla n >= 1
slowo2(S) :- slowo2(S, []).
slowo2([a | L], A) :- slowo2(L, [b | A]).
slowo2([b | S], [b | S]).

%slowo3(S, R) <=> S = a^n b^n R
slowo3([a | L], R) :- slowo3(L, [b | R]).
slowo3(R, R).



max2(L) :- max2(L, _, _).
max2([], _X, _Y).
max2([X | L], X, Y) :- !, max2(L, X, Y).
max2([Y | L], X, Y) :- max2(L, X, Y).
