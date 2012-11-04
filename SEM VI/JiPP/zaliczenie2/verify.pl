% Wojciech Żółtak (wz292583) - Prolog, program zaliczeniowy
%
% Ponizszy program zawiera implementacje naiwnego algorytmu poszukujacego
% zlych przeplotow w danych programach.
%
% Jako pamiec uzywane sa mapy w postaci list par p(Indeks, Wartosc).
% W przypadku tablic, zmienna zawierajaca tablice, jako wartosc posiada
% wlasna mape.
%
% Stan opisywany jest przez symbole funkcyjne
% state(Counters, History, Memory) oraz state(Counter, Memory),
% gdzie:
%     Counters - lista licznikow intrukcji poszczegolnych procesow
%     History  - dotychczasowy przeplot wykonania w postaci listy roznicowej
%     Memory   - lista uproszczonych stanow (bez historii), ktore wystapily
%                w danym przeplocie.


% Operator nierownosci z naszego pseudojezyka.
:- op(700, xfx, '<>').
% Konstruktor list roznicowych.
:- op(700, xfx, '--').

% set_val(+S1, +Var, +Val, -S2) <=> S2 zawiera wszystkie elementy S1,
% 			            wyjąwszy element postaci p(V, _),
% 			            S2 zawiera dodatkowo p(Var, Val).
%                                   and p(Var, Val) in S2
% Zakłada, że w S1 jest dokładnie jeden element postaci p(V, _).
set_val([p(Var, _) | L], Var, Val, [p(Var, Val) | L]).
set_val([p(Var2, Val2) | L], Var, Val, [p(Var2, Val2) | S]) :-
        Var2 \= Var,
        set_val(L, Var, Val, S).

% get_arr(+S, +V, +I, -X) <=> p(V, A) jest elementem  S,
%                             a p(I, X) jest elementem A
% Zakłada, że w S oraz A nie ma dwóch elementów p(A, _),
% p(B, _) takich, że B = A.
get_arr(S, V, I, X) :-
	member(p(V, A), S), !,
	member(p(I, X), A).

% set_arr(+S, +Var, +I, +Val, -S2) <=> S2 zawiera wszystkie elementy S
%                                      za wyjątkiem elementu p(Var, X)
%                                      oraz dodatkowo zawiera p(Var, Y)
%                                      taki, że zachodzi set_val(X, I, Val).
%
% Zakłada, że w S oraz A nie ma dwóch elementów p(A, _),
% p(B, _) takich, że B = A.
set_arr(S, Var, I, Val, S2) :-
	member(p(Var, A), S),
        set_val(A, I, Val, A2),
        set_val(S, Var, A2, S2).

% expr(+E, +S, -N) <=> N jest wartością wyrażenia E w stanie S pamieci.
expr(N, _, N) :- number(N).
expr(arr(Var, Ix), S, Val) :-
        expr(Ix, S, Ix2),
        get_arr(S, Var, Ix2, Val).
expr(Var, S, Val) :-
	atom(Var),
	member(p(Var, Val), S).
expr(E1 + E2, S, N) :-
	expr(E1, S, N1),
	expr(E2, S, N2),
	N is N1 + N2.
expr(E1 - E2, S, N) :-
	expr(E1, S, N1),
	expr(E2, S, N2),
	N is N1 - N2.
expr(E1 * E2, S, N) :-
	expr(E1, S, N1),
	expr(E2, S, N2),
	N is N1 * N2.
expr(E1 / E2, S, N) :-
	expr(E1, S, N1),
	expr(E2, S, N2),
	N is N1 / N2.

expr(E1 < E2, S, true) :-
	expr(E1, S, N1),
	expr(E2, S, N2),
	N1 < N2.
expr(E1 < E2, S, false) :-
	expr(E1, S, N1),
	expr(E2, S, N2),
	N1 >= N2.

expr(E1 > E2, S, true) :-
	expr(E1, S, N1),
	expr(E2, S, N2),
	N1 > N2.
expr(E1 > E2, S, false) :-
	expr(E1, S, N1),
	expr(E2, S, N2),
	N1 =< N2.

expr(E1 = E2, S, true) :-
	expr(E1, S, N1),
	expr(E2, S, N2),
	N1 =:= N2.
expr(E1 = E2, S, false) :-
	expr(E1, S, N1),
	expr(E2, S, N2),
	N1 =\= N2.

expr(E1 <> E2, S, true) :-
	expr(E1, S, N1),
	expr(E2, S, N2),
	N1 =\=  N2.
expr(E1 <> E2, S, false) :-
	expr(E1, S, N1),
	expr(E2, S, N2),
	N1 =:= N2.


% nth(+N, +L, -X) <=> X jest N-tym elementem L (indeksacja od 1)
nth(1, [X | _], X).
nth(N, [_ | L], X) :-
	N2 is N - 1,
	nth(N2, L, X).


% instr(+I, +C1, +S1, +BC, -NC, -S2) <=> NC oraz S2 sa odpowiednio licznikiem
%                                        instrukcji oraz stanem pamieci po wykonaniu
%                                        instrukcji I z licznikiem C1 w stanie S1,
%                                        dla programu o BC instrukcjach
instr(assign(Var, E), C1, S1, BC, C2, S2) :-
        atomic(Var),
        expr(E, S1, N),
        set_val(S1, Var, N, S2),
        C2 is C1 mod BC + 1.
instr(assign(arr(Var, Ix), E), C1, S1, BC, C2, S2) :-
        expr(Ix, S1, Ix2),
        expr(E, S1, N),
        set_arr(S1, Var, Ix2, N, S2),
        C2 is C1 mod BC + 1.
instr(goto(N), _, S, _, N, S).
instr(condGoto(E, N), _, S, _, N, S) :-
        expr(E, S, true).
instr(condGoto(E, _), C1, S, BC, C2, S) :-
        expr(E, S, false),
        C2 is C1 mod BC + 1.
instr(sekcja, C1, S, BC, C2, S) :-
        C2 is C1 mod BC + 1.

% multi_n(Len, N, Lst) <=> Lst jest lista zawierajaca Len razy element N.
multi_n(0, _, []).
multi_n(Len, N, [N | Lst]) :-
        Len > 0,
        Len2 is Len - 1,
        multi_n(Len2, N, Lst).

% init_arr(K, N, Arr) <=> Gdy Arr jest wyzerowana tablica indeksowana
%                         od K do N-1.
init_arr(N, N, []).
init_arr(K, N, [p(K, 0) | Lst]) :-
        K =< N,
        K2 is K + 1,
        init_arr(K2, N, Lst).

% clean_mem(+P, +N, -S) <=> S zawiera wyzerowana pamiec dla programu P
%                           przy N procesach.
clean_mem(P, N, S) :- clean_mem(P, N, [], S).
clean_mem([], _, Acc, Acc).
clean_mem([assign(Var, _) | L], N, Acc, S) :-
        atomic(Var),
        ( member(p(Var, _), Acc) ->
                clean_mem(L, N, Acc, S)
                ;
                clean_mem(L, N, [p(Var, 0) | Acc], S) ).
clean_mem([assign(arr(Var, _), _) | L], N, Acc, S) :-
        ( member(p(Var, _), Acc) ->
                clean_mem(L, N, Acc, S)
                ;
                init_arr(0, N, Val),
                clean_mem(L, N, [p(Var, Val) | Acc], S) ).
clean_mem([X | L], N, Acc, S) :-
        X \= assign(_, _),
        clean_mem(L, N, Acc, S).

% base_state(+P, +N, -S) <=> S zawiera startowy stan dla programu P uruchomionego
%                            w N kopiach.
base_state(P, N, S) :-
        multi_n(N, 1, Cnt),
        Hst = (X -- X),
        clean_mem(P, N, Mem),
        S = state(Cnt, Hst, [p(id, -1) | Mem]).

% step(+BP, +S1, +S2) <=> S2 jest stanem po wykonaniu jednej instrukcji programu BP
%                         przez dowolny z procesow, w stanie S1.
step(BP, S1, S2) :-
        length(BP, BC),
        step_help(0, S1, BP, BC, S2).

step_help(K, state([C | Cnt], (HH -- HT), Store), BP, BC, State2) :-
        set_val(Store, id, K, S),
        nth(C, BP, Instr),
        instr(Instr, C, S, BC, C2, Store2),
	set_val(Store2, id, -1, StoreFinal),
        HT = [K | HT2],
        State2 = state([C2 | Cnt], (HH -- HT2), StoreFinal).
step_help(K, state([C | Cnt], Hst, Store), BP, BC, State2) :-
        K2 is K + 1,
        step_help(K2, state(Cnt, Hst, Store), BP, BC, state(Cnt2, Hst2, Store2)),
        State2 = state([C | Cnt2], Hst2, Store2).

% in_section(Cnt, Prog, K) <=> Dla licznikow instrukcji Cnt i programu Prog
%                              K procesow jest w sekcji krytycznej.
in_section([], _, 0).
in_section([C | Cnt], Prog, K) :-
	nth(C, Prog, sekcja),
	!,
	in_section(Cnt, Prog, L),
	K is L + 1.
in_section([C | Cnt], Prog, K) :-
	nth(C, Prog, Instr),
	Instr \= sekcja,
	in_section(Cnt, Prog, K).

% print_conflicting(Cnt, Prog) :- Dla licznikow Cnt i programu Prog, wypisuje
%                                 numer procesu będącego w sekcji krytycznej.
print_conflicting(Cnt, Prog) :-
	print('Procesy w sekcji:'), nl,
	print_conflicting(Cnt, Prog, 0).

print_conflicting([], _, _).
print_conflicting([C | Cnt], Prog, Id) :-
	nth(C, Prog, sekcja),
	!,
	print('\t'), print(Id), nl,
	NextId is Id + 1,
	print_conflicting(Cnt, Prog, NextId).
print_conflicting([C | Cnt], Prog, Id) :-
	nth(C, Prog, Instr),
	Instr \= sekcja,
	NextId is Id + 1,
	print_conflicting(Cnt, Prog, NextId).

% bad_state(State, Program) <=> State jest złym stanem dla danego Programu.
bad_state(state(Cnt, (Hst -- []), _), Prog) :-
	in_section(Cnt, Prog, K),
	K > 1,
	print_conflicting(Cnt, Prog), nl,
	print('Zly przeplot:'), nl,
	print('\t'), print(Hst), nl.


% may_fail(Prog, N) <=> Program Prog dzialajcy w N kopiach posiada zly przelot.
may_fail(Prog, N) :-
        base_state(Prog, N, state(Cnt, Hst, Store)),
        naive_search(Prog, state(Cnt, Hst, Store), [state(Cnt, Store)]).

% naive_search(Prog, State, Memory) <=> Program Prog w stanie State, z historia
%                                       stanow Memory posiada zly przeplot
%                                       w dalszym dzialaniu.
naive_search(Prog, State, _Hst) :-
        bad_state(State, Prog).
naive_search(Prog, State, Memory) :-
        step(Prog, State, state(Cnt, Hst, Store)),
        \+ member(state(Cnt, Store), Memory),
        naive_search(Prog, state(Cnt, Hst, Store), [state(Cnt, Store) | Memory]).


user:runtime_entry(start):-
     current_prolog_flag(argv, [Arg1, Arg2]) ->
         see(Arg2),
         read(Program),
         seen,
         atom_chars(Arg1, Chars),
         number_chars(N, Chars),
         may_fail(Program, N)
        ;
         write('Niepoprawne wywolanie, usage: verify N file').
