(* strAnagramHash
   modul do tworzenia (niejednoznacznych!) hashy dla stringow o stalej dlugosci
   
   Specjalne wlasciowosci:
   - jadro funkcji [hash] jest relacja rownowaznosci "anagram" (stad niejedno-
   znacznosc hashy)
   - porownywanie hashy w czasie stalym

   *)
   
type t

val hash : string -> t
(** [hash s] zwraca hash stringa [s] *)

val compare : t -> t -> int
(** [compare h1 h2] porownuje dwa hashe zwracajac wartosc analogiczna
   jak funkcja Pervasives.compare *)

