(*
  Kolejka priorytetowa
  Implementacja na drzewach lewicowych

  Wojciech Żółtak
*)

type 'a queue = Leaf | Node of 'a queue * 'a * int * 'a queue;;

let empty = Leaf;;

exception Empty;;

let is_empty q1 =
  match q1 with
  | Leaf -> true
  | _ -> false;;

let root_of q1 = 
  match q1 with 
  | Leaf -> raise Empty 
  | Node (_, x, _, _) -> x;;
(** root_of : 'a queue -> 'a
pomocniczy selektor zwracająca korzeń *)

let height_of q1 = 
  match q1 with 
  | Leaf -> 0
  | Node (_, _, x, _) -> x;;
(** height_of : 'a queue -> int
pomocniczy selektor zwracający długość skrajnie prawej ścieżki *)

let childs_of q1 =
  match q1 with
  | Leaf -> raise Empty
  | Node(l, _, _, r) -> (l, r);;
(** childs_of : 'a queue -> ('a queue * 'a queue)
pomocniczy selektor zwracający poddrzewa drzewa *)

let sort_queue q1 q2 = 
  if root_of q1 > root_of q2 then (q2, q1)
  else (q1, q2);;
(** sort_queue : 'a queue -> 'a queue -> ('a queue * 'a queue)
pomocnicza funkcja sortująca dwie kolejki po wartości w korzeniu *)

(*
  Algorytm łączenia zgodnie z zaleceniemi z treści zadania
  przez dobór nowego korzenia, rekurencyjne złączanie poddrzew
  po prawych gałęziach i poprawianie ich kolejności.
  Złożoność czasowa - liniowa względem 'prawych wysokości'
  łączonych drzew.
*)
let rec join q1 q2 = 
  if is_empty q2 then q1 
  else if is_empty q1 then q2
  else
    let (d1, d2) = sort_queue q1 q2 in
    let new_root = root_of d1 in
    let (d1l, d1r) = childs_of d1 in
    let d3 = join d1r d2 in
    let d1l_h = height_of d1l in
    let d3_h = height_of d3 in
    if d1l_h > d3_h then
      Node(d1l, new_root, (d3_h + 1) , d3)
    else
      Node(d3, new_root, (d1l_h + 1) , d1l);;

let add el q1 = 
  let q2 = Node(Leaf, el, 1, Leaf) in 
  join q1 q2;;

let delete_min q = 
  if is_empty q then raise Empty
  else let root = root_of q in
    let (t1, t2) = childs_of q in
    (root, join t1 t2);;
