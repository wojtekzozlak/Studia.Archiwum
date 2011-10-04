type t = (char * int) list;;
(** typ hasha *)


(* [hash st] zwraca hash ciagu znakow w postaci listy par (znak, krotnosc)
   uporzadkowanej alfabetycznie. Pary (x, 0) zostaja pominiete.
   
   Dziala na zasadzie prostego imperatywnego sortowania przez zliczanie.
   
   Slownik nad ktorym jest string jest staly (znaki ASCII), wiec zlozonosc
   czasowa - Teta(n) wzgledem dlugosci [s]
   
   zlozonosc pamieciowa O(1), bo hash o dlugosci wiekszej niz tablica ASCII *)
let hash string =
  let tab = Array.make 255 0 in
  let length = (String.length string) in
  let char_k = ref 0 in
  let rec peon n acc =
    if n = 0 then acc
    else
      if tab.(n) <> 0 then
        let el = ( (Pervasives.char_of_int n), tab.(n) ) in
        peon (n-1) (el::acc)
      else peon (n-1) acc
  in
    for i = 0 to length - 1  do
      char_k := Pervasives.int_of_char string.[i];
      tab.( !char_k )  <-  (tab.( !char_k ) + 1) ;
    done ;
    peon 254 [];;

(* [compare h1 h2] zwraca relacje w porzadku liniowym dwoch hashy,
   analogicznie jak funkcja Pervasives.compare. Tu akurat jest nia
   sama Pervasives.compare.
   
   Dlugosc hasha mniejsza od stalej, wiec porownanie w czasie stalym. *)
let compare = Pervasives.compare;;