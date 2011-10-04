(* funkcja identycznosciowa przekazywana do scanfa *)
let id x = x;;

(* funkcja wypisujace liste stringow, oddzielajac je spacjami,
   konczac wszystko znakim konca linii *)
let wypisz lista = 
  List.iter (fun el -> print_string el ; print_string " ") lista;
  print_string "\n";;

(* funkcja wczytujaca slowa z wejscia dopoki cos na nim czeka,
   wrzucajace je do zbioru porzadkujacego na klasy abstrakcji
   w relacji anagram, a nastepnie wypisujaca wszystko na standardowe 
   wyjscie *)
let rec wczytywanie k =
  let slowo = Scanf.scanf " %s" id in
  if slowo = "" then List.iter wypisz (Klasy.klasy k)
  else wczytywanie (Klasy.wstaw_slowo slowo k);;
  
wczytywanie (Klasy.puste StrAnagramHash.hash StrAnagramHash.compare);;