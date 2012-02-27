uzg([]).
uzg([X]).
uzg([X, X | Y]) :- uzg([X | Y]).
uzg([X | Y], X) :- uzg([X | Y]).


% porzadek(X, Y) wtw X <= Y
porzadek(a, a).
porzadek(a, b).
porzadek(a, c).
porzadek(b, b).
porzadek(b, c).

fp([X | L], [Y, S]) :- 
