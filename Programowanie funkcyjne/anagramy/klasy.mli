(* klasy.ml
   Modul pozwala budowac uporzadkowany zbior klas abstrakcji slow
   w relacji rownowaznosci bedacej jadrem zadanej funkcji typu [string -> 'a].
   Z powodow oszczednosciowych wymaga do dzialania funkcji porownujacej
   odpowiadajacej jakiemus porzadkowi liniowemu w type ['a].
   
   Operacje wstawiania slowa dzialaja w czasie O(Q * log m + k * log n) gdzie 
   [Q] to zlozonosc czasowa porownania dwoch obiektow typu ['a], [m] iloscia
   klas abstrakcji w zbiorze, [n] rozmiar klasy abstrakcji, do ktorej
   trafia slowo, a [k] dlugosc samego slowa.
   
   Operacja wypisania dziala w czasie O(m * log m) wzgledem ilosci klas
   w zbiorze
   
   Operacja zwrocenia ilosci klas w Teta(1)  *)
      
type 'a t

val puste : (string -> 'a) -> ('a -> 'a -> int) -> 'a t
(** [puste f cmp] zwraca pusty zbior klas abstrakcji relacji rownowaznosci
    bedacej jadrem funkcji [f] wykorzystujac funkcje porownujaca [cmp] do
    optymalizacji operacji wstawiania slow *)

val wstaw_slowo : string -> 'a t -> 'a t
(** [wstaw_slowo slowo zbior] zwraca zbior klas abstrakcji zawierajcy wszystkie
    obiekty z [zbior] oraz [slowo] umieszczone w odpowiedniej klasie. *)

val klasy : 'a t -> (string list) list
(** [klasy zbior] zwraca liste list elementow klas, posortowana alfabetycznie
    w obrebie klas oraz alfabetycznie po klasach wzgledem "najmniejszego" 
    slowa w danej klasie. *)
    
val ilosc : 'a t -> int
(** [ilosc zbior] zwraca ilosc klas abstrakcji w danym zbiorze [zbior] *)
