type 'a queue = Leaf | Node of 'a queue * 'a * int * 'a queue

let empty = Leaf

exception Empty


let root_of q1 = 
    match q1 with 
    | Leaf -> raise Empty 
    | Node (_, x, _, _) -> x

let height_of q1 = 
    match q1 with 
    | Leaf -> 0
    | Node (_, _, x, _) -> x;;

let sort_queue q1 q2 = 
    if root_of q1 > root_of q2 then (q2, q1)
    else (q1, q2)

let is_empty q1 =
    match q1 with
    | Leaf -> true
    | _ -> false

let childs_of q1 =
    match q1 with
    | Leaf -> raise Empty
    | Node(l, _, _, r) -> (l, r)

(* złączenie dwóch kolejek typu 'a *)
let rec join q1 q2 = 
   if is_empty q1 && is_empty q2 then q1
   else if is_empty q2 && not (is_empty q1) then q1
   else if is_empty q1 && not (is_empty q2) then q2
   else let (d1, d2) = sort_queue q1 q2 in
       let (d1l, d1r) = childs_of d1 in
       let d3 = join d1r d2 in
       let h_d1l = height_of d1l in
       let h_d3 = height_of d3 in
           if h_d1l > h_d3 then Node(d1l, root_of d1, (1 + min h_d1l h_d3) , d3)
           else Node(d3, root_of d1, (1 + min h_d1l h_d3) , d1l) ;;

let add q1 el = 
    let q2 = Node(Leaf, el, 1, Leaf) in 
    join q1 q2

let delete_min q = 
    if is_empty q then raise Empty
    else let root = root_of q in
         let (t1, t2) = childs_of q in
         (root, join t1 t2)
