% nat(k) wtw gdy k jest liczbą naturalną
% stała : 0, symbol funkcyjny s/1
% 0, s(0), s(s(0)), ...  termy

nat(0).
nat(s(X)) :- nat(X).

% plus(X, Y, Z) <=> X + Y = Z
plus(0, X, X). % dla nat(X)
%plus(X, 0, X).
% plus(X, Y, Z) :- plus(Y, X, Z). [powoduje pętlenie]!!!
plus(s(X), Y, s(Z)) :- plus(X, Y, Z).
