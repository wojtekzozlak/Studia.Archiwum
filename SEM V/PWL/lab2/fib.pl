% fib(k, n) <=> n = k-ta liczba Fib

fib(0, 0).
fib(s(0), s(0)).
fib(s(s(K)), N) :- fib(K, X), fib(s(K), Y), plus(X, Y, N).
