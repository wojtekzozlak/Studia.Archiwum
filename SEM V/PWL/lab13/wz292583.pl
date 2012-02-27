:- op(500, xfx, --).

%bracia(+Drzewo, ?W, ?L, ?P) <=> gdy W jest wierzchołkiem drzewa
%                                L lewy brat (lub null gdy nie ma)
%                                P prawy brat (lub null gdy nie ma)
bracia(D, W, L, P) :- podziel(D, Poz), znajdz(Poz, W, L, P).

%podziel(D, Poz) <=> Poz jest listą list drzew z korzeniami
%                    na tym samym poziomie wejsciowego drzewa oraz nilem przy wierzcholku korzeniu
podziel(D, Poz) :-
    podziel([tree(D, _, nil)], X -- X, Poz).

%podziel(DoPrzerobienia, PoziomNizej, Zebrane)
%
%       <=> DoPrzerobienia jest kolejką w przejściu BFS,
%           PoziomNizej jest wierzcholkami do przejrzenia w nastepnej
%             iteracji
%           Zebrane, to zebrane poziomy wierzcholkow
podziel([], X -- X,  _Poz) :- var(X), !.
podziel([], H -- [], [H | Poz]) :-
    podziel(H, X -- X, Poz).
podziel([ nil | L], H -- T, Poz) :-
    podziel(L, H -- T, Poz).
podziel([ tree(LD, _W, PD) | L], H -- [LD, PD | T], Poz) :-
    podziel(L, H -- T, Poz).

%znajdz(Poz, W, L, P) <=> W ma braci L i P w liscie list poziomow Poz
znajdz([X | Poz], W, L, P) :-
    popraw(X, X2),
    sprawdzWiersz(X2, W, L, P).
znajdz([X | Poz], W, L, P) :-
    znajdz(Poz, W, L, P).

%sprawdzWiersz(X, W, L, P) <=> W ma braci L i P w liscie poziomu X
sprawdzWiersz([X | Poz], W, L, P) :-
    sprawdzWiersz(Poz, W, L, P).

sprawdzWiersz([L, W, P | Poz], W, L, P).

%popraw(X, X2) <=> X2 jest lista X, bez nili, zaczętą i skończoną nullami
%                  i z wartościami korzeni zamiast drzew
%
%  nie zdazylem i wszystko jest okropne i w ogole! ;(
