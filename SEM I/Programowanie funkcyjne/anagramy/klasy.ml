module StringSet = Set.Make(String);;

type 'a rep_fun = string -> 'a;;

(* typ obiektu, struktura zawierajaca funkcje typu [string -> 'a], 
   mape setow stringow indeksowana typem ['a] oraz licznik klas w zbiorze *)
type 'a t = {
  funk : 'a rep_fun;
  zbior :('a, StringSet.t) PMap.t;
  il : int;
};;

(* [puste f por] zwraca strukture reprezentujaca klasy abstrakcji jadra
   funkcji [f] wykorzystujac [por] do efektywnego zarzadzania mapa *)
let puste f por = { funk = f ; zbior = (PMap.create por) ; il = 0 }

(* [wstaw_slowo slowo zbior] zwraca obiekt posiadajacy klasy takie same
   jak [zbior] wzbogacone o [slowo]
   
   funkcja liczy [funk(slowo)] gdzie funk to funkcja zapisana w [zbior]
   a nastepnie poszukuje odpowiadajacej mu klasy abstrakcji, by dodac tam
   [slowo]. W przypadku braku takowej, tworzy mu nowa, zwiekszajac przy tym
   licznik klas w strukturze.
   
   Zlozonosc czasowa: O(Q * log m + k * log n) gdzie [Q] to zlozonosc
   czasowa funkcji [por] uzytej do stworzenia obiektu, [m] ilosc klas
   w zbiorze, [k] dlugosc [slowo], [n] ilosc slow w klasie do ktorej
   trafia [slowo]
   
   Zlozonosc pamieciowa:
    O(log n + log k) gdzie [n] to ilosc klas w zbiorze, a [k] rozmiar klasy
    do ktorej dodajemy slowo (z wlasnosci drzew AVL) *)
let wstaw_slowo slowo { funk = funk ; zbior = zbior ; il = il } =
  let klucz = funk slowo in
  let (klasa, il) = try (PMap.find klucz zbior, il) with
    | Not_found -> (StringSet.empty, il+1)
  in
  let klasa = StringSet.add slowo klasa in
  { funk = funk ; zbior = PMap.add klucz klasa zbior ; il = il}

(* [klasy zbior] zwraca liste list klas ze [zbior] uporzadkowana alfabetycznie
   w obrebie kazdej klasy, oraz alfabetycznie wzgledem "najmniejszego" elementu
   kazdej klasy.
   
   Klasy trzymane na setcie, wiec same z siebie uporzadkowane, pozostaje
   posortowanie listy klas. Do czego uzywamy zwyklego Pervasives.compare.
   
   Zlozonosc czasowa: O(n * k * log k) gdzie [n] to najdluzsze "najmniejsze"
   slowo, natomiast [k] ilosc klas abstrakcji w [zbior].
   
   Zloznosc pamieciowa: O(m) wzgledem sumaraczynej dlugosci wszystkich slow
   w [zbior] *)
let klasy { zbior = zbior } =
  let h war acc =
    (StringSet.elements war)::acc
  in
  let lista = PMap.fold h zbior [] in
  let lista = List.sort Pervasives.compare lista in
  lista;;
  
let ilosc { il = il } = il;;