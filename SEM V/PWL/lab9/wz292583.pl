%Wojciech Żółtak

%Macierz reprezentowana jako lista list liczb o tej samej dlugosci

%kolumna(+Macierz, ?K, ?W) <=> gdy K-ta kolumna zawiera tylko elementy W
kolumna([W], K, E) :-
	kty(W, K, E).
kolumna([W | M ], K, E) :-
	kty(W, K, E),
	kolumna(M, K, E).

%kty(+W, ?K, ?E) <=> E jest K-tym elementem w liscie W
kty(W, K, E) :-
	kty(W, K, 1, E).

%kty(W, K, A, E) <=> E jest (K - A + 1)tym  elementem w liscie W
kty([E | _L], K, K, E).
kty([_X | L], K, A, E) :-
	A2 is A + 1,
	kty(L, K, A2, E).
