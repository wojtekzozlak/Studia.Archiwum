

Random.self_init() ;;
open List;;


let rec mieszaj list = 
    let pair_split list = 
        let h (l1, l2, w) obj = 
             if w then (obj::l1, l2, not w )
             else (l1, obj::l2, not w )
        in let (l1, l2, _) = List.fold_left h ([], [], true) list
        in (l1, l2)
    in match list with
    | [] -> failwith "cos nie tak"
    | h::[] -> [h]
    | h::tl -> let (l1, l2) = pair_split list in
        if Random.bool () then (mieszaj l1) @ (mieszaj l2) 
	else (mieszaj l2)@(mieszaj l1);;

let rec mieszaj_n list n =
    if n = 0 then list
    else let list = mieszaj list in
        mieszaj_n list (n-1);;


let rec list_n n =
    let rec list_n_h list n =
	if n = 0 then list
        else list_n_h (n::list) (n-1)
    in list_n_h [] n;;

let rozloz tree = 
    let rec rozloz_h tree wyn = 
         if is_empty tree then rev wyn
         else let (x, tree) = delete_min tree in
              rozloz_h tree (x::wyn)
    in rozloz_h tree [];;

exception BadHeight of (int*int*int*int);;
exception NonLeft of (int*int*int);;
let is_left_t_debug tree debug = 
  let rec help tree = 
    match tree with
    | Leaf -> 0
    | Node(lt, root, h, rt) -> 
        let lh = help lt in
	let rh = help rt in
	if rh > lh then raise (NonLeft (lh, root, rh))
        else if (rh + 1) <> h then raise (BadHeight (lh, root, h, rh))
	else rh+1
  in if debug then (fun x -> true)(help tree)
  else try (fun x -> true)(help tree) with
  | BadHeight(_,_,_,_) -> false
  | NonLeft(_,_,_) -> false;;

let is_left_t tree = is_left_t_debug tree false;;
let is_left_t_d tree = is_left_t_debug tree true;;

let build_tree list = fold_left (fun tree x -> add tree x) empty list;;


let create_rtree size = 
    let size = (fun x -> if x <= 0 then Random.int 10 else x) size in
    let x = list_n size in
    let x = mieszaj_n x 10 in
build_tree x;;

let is_iter list =
    let rec is_iter_h list last =
        match list with
        | [] -> true
        | h::tl -> if h <> (last+1) then false
                   else is_iter_h tl h
    in match list with
    | [] -> true
    | h::tl -> is_iter_h tl h;;

let queue_test test_size tree_size = 
    let rec queue_test_h size acc =
        if(size = 0) then acc
        else let tree = create_rtree tree_size in
            let iter = is_iter (rozloz tree) in
            let l = is_left_t tree in
            queue_test_h (size-1) ((iter, l)::acc)
    in queue_test_h test_size [];;


let queue_test2 test_size tree_size = 
    let rec queue_test_h size =
        if(size = 0) then true
        else let tree = create_rtree tree_size in
            queue_test_h (size-1)
    in queue_test_h test_size;;



