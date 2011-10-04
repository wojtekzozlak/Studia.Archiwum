	(*
 * PSet - Polymorphic sets
 * Copyright (C) 1996-2003 Xavier Leroy, Nicolas Cannasse, Markus Mottl
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version,
 * with the special exception on linking described in file LICENSE.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *)


(* typ przedziałowy *)
type div = int * int

(* typ implementacji zbioru przedziałów
drzewo binarne plus dane o wysokości i ilości wierzchołków w poddrzewach *)
type t =
  | Empty
  | Node of t * div * t * int * int


(* funkcja porównująca zbiory rozłączne *)
let div_cmp (a1, a2) (b1, b2) = 
  if a2 < b1 then -1
  else if b2 < a1 then 1
  else if a1 = b1 && a2 = b2 then 0
  else failwith "non_disjoint"

let cmp = div_cmp


(* [find_left_bound set div] znajduje skrajnie lewy przedział
dotykający do [div] bądź mający z nim część wspólną,
lub zwraca wyjątek gdy taki nie istnieje

KD - korzeń drzewa ; DP - dany przedział
weźmy KD:
- jeśli w przedziele z KD znajduje się lewy kraniec DP,
albo stykasz się z nim z prawej strony, to jest on lewym kresem
w przeciwnym wypadku
  - jeśli lewy kraniec DP jest na lewo od lewego krańca KD,
  to poszukajmy lewego krańca z lewej
    + jeśli nic nie znaleziono (zwrócono wyjątek), to zastanówmy się, czy
    przedziały się częściowo nie pokrywają. Wtedy to skrajnie lewy przedział
    spełniający tą własność i jest to lewy kres
  - jeśli lewy kraniec DP jest o więcej niż 1 w prawo od prawego krańca KD,
  to pozostało nam szukać na prawo, jeśli tam nic nie znaleziono, to już nic
  nie znajdziemy 
  
Złożoność czasowa - O(log n)
Pamięciowa - O(1) *)
exception No_Bound

let rec find_left_bound set (d1, d2) = 
  match set with
  | Empty -> raise No_Bound
  | Node (ls, (r1, r2), rs, _, _) ->
    if (r1 <= d1 && r2 >= d1) || r2 = (d1-1) then (r1, r2)
    else if r1 > d1 then
      try find_left_bound ls (d1, d2) with
      | No_Bound -> if d2 >= r1 then(r1, r2)
        else raise No_Bound
    else 
      find_left_bound rs (d1, d2)

(* [find_right_bound set div] działa analogicznie jak 
[find_left_bound set div], ale w prawo *)
let rec find_right_bound set (d1, d2) = 
  match set with
  | Empty -> raise No_Bound
  | Node (ls, (r1, r2), rs, _, _) ->
    if (r1 <= d2 && r2 >= d2) || r1 = (d2+1) then (r1, r2)
    else if r2 < d2 then
      try find_right_bound rs (d1, d2) with
      | No_Bound -> if d1 <= r2 then (r1, r2)
        else raise No_Bound
    else 
      find_right_bound ls (d1, d2) 

let find_bounds x set =
let lb = try find_left_bound set x with
  | No_Bound -> x
  in
  let rb = try find_right_bound set x with
  | No_Bound -> x
  in
  (lb, rb)

(* [height set] zwraca wysokość drzewa tworzącego set.
Działanie bez zmian.*)
let height = function
  | Node (_, _, _, h, _) -> h
  | Empty -> 0

(* [make l k r] tworzy poprawny set o lewym poddrzewie [l], prawym [r]
i korzeniu [k]. Rozszerzono o przechowywanie informacji o ilości
elementów w secie. *)
let make l k r = 
  let l_num = match l with
  | Empty -> 0
  | Node (_, (r1, r2), _, _, l_num) -> r2 - r1 + l_num + 1
  in
  let r_num = match r with
  | Empty -> 0
  | Node (_, (r1, r2), _, _, r_num) -> r2 - r1 + r_num + 1
  in
  Node (l, k, r, max (height l) (height r) + 1, (l_num + r_num) )

(* [bal l k r] balansuje set utworzony z [l] [k] [r] tak by maksymalna
różnica wysokości poddrzew nie była większa od 2. Zakładamy, że [l] jest
zrównoważone względem [r]. Funkcja poprawiona o konsekwentrne
korzystanie z make'a*)
let bal l k r =
  let hl = height l in
  let hr = height r in
  if hl > hr + 2 then
    match l with
    | Node (ll, lk, lr, _, _) ->
        if height ll >= height lr then make ll lk (make lr k r)
        else
          (match lr with
          | Node (lrl, lrk, lrr, _, _) ->
              make (make ll lk lrl) lrk (make lrr k r)
          | Empty -> assert false)
    | Empty -> assert false
  else if hr > hl + 2 then
    match r with
    | Node (rl, rk, rr, _, _) ->
        if height rr >= height rl then make (make l k rl) rk rr
        else
          (match rl with
          | Node (rll, rlk, rlr, _, _) ->
              make (make l k rll) rlk (make rlr rk rr)
          | Empty -> assert false)
    | Empty -> assert false
  else make l k r

(* [min_elt set] zwraca najmniejszy element z [set].
Funkcja pozostawiona bez zmian. *)
let rec min_elt = function
  | Node (Empty, k, _, _, _) -> k
  | Node (l, _, _, _, _) -> min_elt l
  | Empty -> raise Not_found

(* [remove_min_elt set] zwraca [set] z wyłączeniem
z niego najmniejszego elementu. Funkcja pozostawiona
bez zmian. *)
let rec remove_min_elt = function
  | Node (Empty, _, r, _, _) -> r
  | Node (l, k, r, _, _) -> bal (remove_min_elt l) k r
  | Empty -> invalid_arg "PSet.remove_min_elt"

(* [merge t1 t2] łączy dwa sety [t1] [t2] zakładając,
że są zbalansowane. Funkcja pozostawiona bez zmian. *)
let merge t1 t2 =
  match t1, t2 with
  | Empty, _ -> t2
  | _, Empty -> t1
  | _ ->
      let k = min_elt t2 in
      bal t1 k (remove_min_elt t2)

(* pusty set *)
let empty = Empty 

(* [is_empty x] sprawdza, czy [x] jest pustym zbiorem *)
let is_empty x = 
  x = Empty

(* [add_one div set] dodaje przedział [div] do zbioru [set]
zakładając, że [div] jest rozłączny z wszystkimi przedziałami
w zbiorze. Funkcja pozostawiona bez zmian. *)
let rec add_one x = function
  | Node (l, k, r, h, _) ->
      let c = cmp x k in
      if c = 0 then make l x r
      else if c < 0 then
        let nl = add_one x l in
        bal nl k r
      else
        let nr = add_one x r in
        bal l k nr
  | Empty -> make Empty x Empty

(* [join l v r] zwraca set zawierający elementy z [l] [r] oraz
przedział [v]. Funkcja pozostawiona bez zmian. *)
let rec join l v r =
  match (l, r) with
    (Empty, _) -> add_one v r
  | (_, Empty) -> add_one v l
  | (Node(ll, lv, lr, lh, _), Node(rl, rv, rr, rh, _)) ->
      if lh > rh + 2 then bal ll lv (join lr v r) else
      if rh > lh + 2 then bal (join l v rl) rv rr else
      make l v r

(* [split_simple div set] zwraca set zawierający
elementy mniejsze od [div], wartość logiczną oznaczającą
istnienie [div] w zbiorze [set], oraz set zawierający
elementy większe od [div] - całość w postaci trójki
zakładamy, że [div] jest rozłączny z wszystkimi przedziałami
w [set]. Funkcja jest pierwotną funkcją split, ze zmienioną
nazwą. *)
let split_simple x set =
  let rec loop x = function
      Empty ->
        (Empty, false, Empty)
    | Node (l, v, r, _, _) ->
        let c = cmp x v in
        if c = 0 then (l, true, r)
        else if c < 0 then
          let (ll, pres, rl) = loop x l in (ll, pres, join rl v r)
        else
          let (lr, pres, rr) = loop x r in (join l v lr, pres, rr)
  in
  let setl, pres, setr = loop x set in
  setl, pres, setr

(* [add x set] zwraca set zawierający wszystkie elementy 
z [set] oraz element [x].

Niech lewy/prawy kres będzie najmniejszym, największym/najmniejszym
elementem w zbiorze, który dotyka danego przedziału, bądź ma z nim 
część wspólną.

Szukamy lewych i prawych kresów w zbiorze, względem przedziału [x],
a następnie korzystając z funkcji split, dzielimy względem nich [set]
uzyskując zbióry elementów większych i mniejszych od [x] z dokładnością
do kresów. Tworzę nowy przedział [x'] z [x] oraz kresów, a następnie z tych
3 klocków (mniejsze, [x'], większe) tworzę nowy set za pomocą join.

złożoność czasowa - O(log n) {kresy O(log n), splity O(log n), join O(log n) }
*)
let add x set =
  let (lb, rb) = find_bounds x set in
  if lb = x && rb = x then add_one x set
  else
    let (lt, _, rt) = split_simple lb set in
    let (_, _, rt) = split_simple rb rt in
    let x = (min (fst lb) (fst x), max (snd rb) (snd x)) in 
    join lt x rt

(* [mem int set] sprawdza czy liczba całkowita [int] należy do zbioru *)
let mem (x:int) set =
  let rec loop = function
    | Node (l, k, r, _, _) ->
        (fst k <= x && x <= snd k) || loop (if x < fst k then l else r)
    | Empty -> false in
  loop set

let exists = mem


(* [join_simple s1 s2] łączy dwa sety, wyciągając z drugiego minimalny
element i korzystając ze zwykłego join.

Złożoność czasowa - O(log n) przez min_elt, remove_min_elt i join *)
let rec join_simple s1 s2 = 
  match (s1, s2) with
  | (Empty, _) -> s2
  | (_, Empty) -> s1
  | (s1, s2) -> 
    let r = min_elt s2 in
    let s2 = remove_min_elt s2 in
    join s1 r s2

(* [remove x set] zwraca zbiór z elementami należacymi do [set]
z wyłączeniem elementów z przedziału [x].

Szukamy lewego i prawego kresu względem [x].
Znajdujemy sety elementów mniejszych oraz większych od kresów
(bądź samego [x] jak takowe nie istnieją).
Łączymy je, a następnie dokładamy do nich kresy bez części wspólnej z [x] 

Złożoność czasowa - O(log n) ze względu na kresy, splity oraz addy *)
let remove x set = 
  let (lb, rb) = find_bounds x set in
  let (lt, _, rt) = split_simple lb set in
  let (_, _, rt) = split_simple rb rt in
  let set = join_simple lt rt in
  let set = if fst lb < fst x then add (fst lb, (fst x ) - 1) set 
    else set in
  let set = if snd rb > snd x then add ((snd x) + 1 , snd rb) set
    else set in
  set

(* [split int set] zwraca set elementów mniejszych od [int], wartość logiczną
opisującą, czy [int] należy do zbioru, oraz set elementów większych. 

Jak zwykle szukamy kresów, ale względem [ (int, int) ].
Splitujemy uzyskując zbiór mniejszych i większych od niego
z dokładnością do kresów. Dodajemy do nich elementy z kresów
mniejsze/większe od [int], a następnie zwracamy rządaną trójkę
biorąc jeszcze wartość z mem.

Złożoność czasowa - O(log n) ze względu na kresy, split i mem *)
let split x set = 
  let (lb, rb) = find_bounds (x,x) set in
  let (lt, _, rt) = split_simple lb set in
  let (_, _, rt) = split_simple rb rt in
  let lt = if fst lb < x then add (fst lb, x - 1) lt
    else lt
  in
  let rt = if snd rb > x then add (x + 1 , snd rb) rt
    else rt
  in
  (lt, (mem x set), rt)

(* [elt_num set] zwraca ilość elementów w [set] 

Złożoność czasowa O(1) co po prostu widać. *)
let elt_num = function
  | Empty -> 0
  | Node(_, (k1,k2), _, _, n) -> k2 - k1 + 1 + n

(* [below x set] zwraca ilość elementów w [set] mniejszych bądź równych [x].

Zakładamy, że wszystkie elementy z setu spełniają nierówność.
Następnie przechodząc po drzewie odcinając poddrzewa większe od [x] usuwamy
z puli ich liczebność. Gdy trafiamy do liścia zwracamy wynik,
gdy do przedziału zawierającego [x] wynik zmieniony o ilość elementów w prawym
poddrzewie i elementy większe z korzenia.

Złożoność czasowa - O(log n) bo jest max log n iteracji a każda wykonuje się
w czasie stałym *)
let below x set =
  match set with
  | Empty -> 0
  | Node(lt, (k1,k2), rt, _, n) ->
    let num = elt_num set in
    let rec helper set il = 
      match set with 
      | Empty -> il
      | Node(lt, (k1,k2), rt, _, n) ->
        if k1 <= x && x <= k2 then helper lt (il - (k2 - x) - elt_num rt)
        else if x < k1 then helper lt (il - (k2 - k1 + 1) - elt_num rt)
        else helper rt il
    in helper set num
    
(* [iter f set] aplikuje funkcję [f] do każdego elementu z [set]. Są one
przekazywane do [f] w kolejności rosnącej. Funkcja bez zmian. *)
let iter f set =
  let rec loop = function
    | Empty -> ()
    | Node (l, k, r, _, _) -> loop l; f k; loop r in
  loop set

(* [fold f set] zwraca [(f xN ... (f x2 (f x1 a))...)] gdzie x1 ... xn
są kolejnymi elementami z [set] w kolejności rosnącej. Funkcja bez zmian. *)
let fold f set acc =
  let rec loop acc = function
    | Empty -> acc
    | Node (l, k, r, _, _) ->
          loop (f k (loop acc l)) r in
  loop acc set

(* [elements set] zwraca listę elementów z [set] w kolejności rosnącej.
Funkcja bez zmian. *)
let elements set = 
  let rec loop acc = function
      Empty -> acc
    | Node(l, k, r, _, _) -> loop (k :: loop acc r) l in
  loop [] set
