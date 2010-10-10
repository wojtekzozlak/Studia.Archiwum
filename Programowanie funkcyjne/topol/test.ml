open Scanf;;

module Iset = Set.Make ( struct type t = int let compare = compare end );;

let rec print_int_list list =
  match list with
    | [] -> () 
    | hd::tl -> print_int hd ; print_string " " ; print_int_list tl;;

let id x = x;;

let rec exists el list = 
  match list with
  | [] -> false
  | hd::tl ->
    if hd = el then true
    else exists el tl;; 

exception NotFound;;

let rec grab_list el list =
  match list with
  | [] -> []
  | (cand, list)::tl ->
    if cand = el then list
    else grab_list el tl;;

let rec check data out =
  match out with
  | [] -> true
  | hd::tl ->
    let list = grab_list hd data in
    let flag = List.fold_left (fun acc el -> acc && (exists el tl) ) true list in
    if flag then check data tl
    else false;;

let check_unique out =
  let out = List.sort compare out in
  let (b, _) = List.fold_left (fun (flag, prev) el -> ((flag || (prev = el)), el) ) (false, (-1) ) out in
  not b;;

let check_all data out =
  let helper acc (el, list) =
    let acc = Iset.add el acc in
    List.fold_left (fun a l -> Iset.add l a) acc list
  in
  let set = Iset.empty in
  let set = List.fold_left helper set data
  in (Iset.elements set = List.sort compare out);;

let is_non el =
  match el with
  | Some(x) -> false
  | None -> true;;


let one_test length = 
  let rec line_grab line_l acc = 
    if line_l = 0 then List.rev acc
    else
      let el = scanf " %d" id in
      line_grab (line_l-1) (el::acc)
  in
  let data_grab l =
    let rec helper l acc =
      if l = 0 then List.rev acc
      else 
        let ver = scanf " %d" id in
	let d_len = scanf " %d" id in
        helper (l-1) (( (ver, line_grab d_len []) )::acc)
    in helper l []
  in
  let data = data_grab length in
  let cyklicznosc = scanf " %d" id in
  let result = try Some(Topol.topol data) with 
    | Topol.Cykliczne -> None
  in
    match result with
    | None -> if cyklicznosc = 1 then (print_string "cyklicznie " ; print_string "test ok")
      else print_string "test fail!"
    | Some(x) -> if check data x then print_string "test ok"
      else print_string "test fail!";;

    



let rec testing l =
  if l = 0 then ()
  else
    let test_l = scanf " %d" id in
    one_test test_l ; print_string "\n" ; testing (l-1);;

let l = scanf " %d" id ;;

testing l;;