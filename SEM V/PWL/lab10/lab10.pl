% liscie(D, L) <=> L jest lista wszystkich lisci D (od lewej do prawej)
liscie(D, L) :- liscie(D, [], L).

liscie(nil, A, A).
liscie(tree(nil, W, nil), A, [W | A]).
liscie(tree(L, _, R), A, W) :- (L, R) \= (nil, nil), liscie(R, A, A2), liscie(L, A2, W).


% odd(X) <=> X jest nieparzyste
odd(s(X)) :- even(X).

% even(X) <=> X jest parzyste
even(0).
even(s(s(X))) :- even(X).

odd2(X) :- \+ even(X).
even2(X) :- \+ odd(X).


even3(0).
even3(s(X)) :- \+ even3(X).

odd3(s(X)) :- \+ odd(X).



% mamy dany predykat wf(x, y) <=> f(x)=y
%
% fna(D) <=> wf jest funkcja 'na' dla jakiejs dziedziny
fna(D) :- fna(D, D).
fna([], D).
fna([X | L], D) :- wf(X, Y), member(Y, D), fna(L, D).
