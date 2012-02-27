:- op(500, xfx, --).

%fp(L, F) <=> L lista z elementow b oraz c,
%             F jest uporzadkowanym L
fp(L, H) :- podziel(L, H -- []).

podziel([], H -- H).
podziel([b | L], [b | H] -- X) :- podziel(L, H -- X).
podziel([c | L], H -- X) :- podziel(L, H -- [c | X]).


fp2(L, F) :- podziel2(L, X -- X, F).

podziel2([], F -- [], F).
podziel2([b | L], H -- T, F) :- podziel2(L, [b | H] -- T, F).
podziel2([c | L], H -- [c | X], F) :- podziel2(L, H -- X, F).


%wszerz(DrzewoBinarne, ListaWierzchWszerz) <=> ListaWierzchWszerz jest listą wierzchołków w przejściu BFS
wszerz(D, L) :- wszerz2([D | X] -- X, L).

wszerz2(X -- _, L) :- var(X), !, L = [].
wszerz2([nil | RK] -- KK, L) :- wszerz2(RK -- KK, L).
wszerz2([tree(L, W, P) | RK] -- [L, P | K], [W | RL]) :-
	wszerz2(RK -- K, RL).


%fh(ListaRWB, RWB) <-> flaga holenderska
fh(L, FH) :- podzielH(L, FH -- C, C).

podzielH([], H -- H, []).
podzielH([a | L], [a | H] -- X, C) :-
	podzielH(L, H -- X, C).

podzielH([b | L], H -- X, C) :-
	podzielH(L, H -- [b | X], C).

podzielH([c | L], H -- X, [c | C]) :-
	podzielH(L, H -- X, C).


fh2(L, RWB):- podzielH2(L, RWB -- WB, WB -- B, B -- []).

podzielH2([], X -- X, Y -- Y, Z -- Z).
podzielH2([r | L], [r | RH] -- RT, WH -- WT, BH -- BT) :- podzielH2(L, RH -- RT, WH -- WT, BH -- BT).
podzielH2([w | L], RH -- RT, [w | WH] -- WT, BH -- BT) :- podzielH2(L, RH -- RT, WH -- WT, BH -- BT).
podzielH2([b | L], RH -- RT, WH -- WT, [b | BH] -- BT) :- podzielH2(L, RH -- RT, WH -- WT, BH -- BT).
