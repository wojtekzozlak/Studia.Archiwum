:- op(100, fy, s).
:- op(200, xfy, <=).

0 <= _X.
s X <= s Y :- X <= Y.

% lista[X] <=> X jest listą
lista([]).
lista([_X | Y]) :- lista(Y).

% pierwszy(E, L) <=> E jest pierwszym eltem L
pierwszy(E, [E | _X]).

% ostatni(E, L) <=> E jest ostatnim eltem L
ostatni(E, [E | [] ]).
ostatni(E, [_X | Y]) :- ostatni(E, Y).

% element(E, L) <=> E jest elementem L
element(E, L) :- pierwszy(E, L).
element(E, [_X | Y]) :- element(E, Y).


% scal(L1, L2, L3) <=> L3 = L1 * L2
scal([], L2, L2).
scal([X|Y], L2, [X|Z]) :- scal(Y, L2, Z).

element2(E, L) :- scal(_, [E|_], L).


% podziel(L, LN, LP) <=> LN lista z indeksami nieparzystymi, LP lista z indeksami parzystymi
podziel([], [], []).
podziel([X | L], [X | LN], LP) :- podziel(L, LP, LN).

% podlista(P, L) <=> spojny podciag
podlista(P, L) :- scal(P, _, L). % prefiksy
podlista(P, [_E | X]) :- podlista(P, X).

podlista2(P, L) :- scal(P, _, X), scal(_, X, L).
