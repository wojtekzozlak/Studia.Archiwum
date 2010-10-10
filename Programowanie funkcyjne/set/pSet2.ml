let string_of_pair (a, b) =
  print_string ("(" ^ (string_of_int a) ^ " , " ^ (string_of_int b) ^ ")") ; ()

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

type div = int * int

type set =
  | Empty
  | Node of set * div * set * int


(* funkcja porównująca zbiory rozłączne *)
let div_cmp (a1, a2) (b1, b2) = 
  if a2 < b1 then -1
  else if b2 < a1 then 1
  else if a1 = b1 && a2 = b2 then 0
  else failwith "non_disjoint"

let cmp = div_cmp


(* KD - korzeń drzewa ; DP - dany przedział
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
  nie znajdziemy *)
(* wyszukiwanie lewego krańca pokrycia drzewa *)
let rec find_left_bound set (d1, d2) = 
  match set with
  | Empty -> failwith "no_bound"
  | Node (ls, (r1, r2), rs, w) ->
    if (r1 <= d1 && r2 >= d1) || r2 = (d1-1) then (r1, r2)
    else if r1 > d1 then
      try find_left_bound ls (d1, d2) with
      | exn -> if d2 >= r1 then(r1, r2)
        else failwith "no_bound"
    else 
      find_left_bound rs (d1, d2)

(* wyszukiwanie lewego krańca pokrycia drzewa *)
let rec find_right_bound set (d1, d2) = 
  match set with
  | Empty -> failwith "no_bound"
  | Node (ls, (r1, r2), rs, w) ->
    if (r1 <= d2 && r2 >= d2) || r1 = (d2+1) then (r1, r2)
    else if r2 < d2 then
      try find_right_bound rs (d1, d2) with
      | exn -> if d1 <= r2 then (r1, r2)
        else failwith "no_bound"
    else 
      find_right_bound ls (d1, d2) 



let height = function
  | Node (_, _, _, h) -> h
  | Empty -> 0

let make l k r = Node (l, k, r, max (height l) (height r) + 1)

let bal l k r =
  let hl = height l in
  let hr = height r in
  if hl > hr + 2 then
    match l with
    | Node (ll, lk, lr, _) ->
        if height ll >= height lr then make ll lk (make lr k r)
        else
          (match lr with
          | Node (lrl, lrk, lrr, _) ->
              make (make ll lk lrl) lrk (make lrr k r)
          | Empty -> assert false)
    | Empty -> assert false
  else if hr > hl + 2 then
    match r with
    | Node (rl, rk, rr, _) ->
        if height rr >= height rl then make (make l k rl) rk rr
        else
          (match rl with
          | Node (rll, rlk, rlr, _) ->
              make (make l k rll) rlk (make rlr rk rr)
          | Empty -> assert false)
    | Empty -> assert false
  else make l k r

let rec min_elt = function
  | Node (Empty, k, _, _) -> k
  | Node (l, _, _, _) -> min_elt l
  | Empty -> raise Not_found

let rec remove_min_elt = function
  | Node (Empty, _, r, _) -> r
  | Node (l, k, r, _) -> bal (remove_min_elt l) k r
  | Empty -> invalid_arg "PSet.remove_min_elt"

let merge t1 t2 =
  match t1, t2 with
  | Empty, _ -> t2
  | _, Empty -> t1
  | _ ->
      let k = min_elt t2 in
      bal t1 k (remove_min_elt t2)

(*let create cmp = { cmp = cmp; set = Empty }*)
let empty = Empty 

let is_empty x = 
  x = Empty

(* dodawanie przedziału rozłącznego z resztą *)
let rec add_one x = function
  | Node (l, k, r, h) ->
      let c = cmp x k in
      if c = 0 then Node (l, x, r, h)
      else if c < 0 then
        let nl = add_one x l in
        bal nl k r
      else
        let nr = add_one x r in
        bal l k nr
  | Empty -> make Empty x Empty

let add x set = add_one x set

let rec join l v r =
  match (l, r) with
    (Empty, _) -> add_one v r
  | (_, Empty) -> add_one v l
  | (Node(ll, lv, lr, lh), Node(rl, rv, rr, rh)) ->
      if lh > rh + 2 then bal ll lv (join lr v r) else
      if rh > lh + 2 then bal (join l v rl) rv rr else
      make l v r

let split x set =
  let rec loop x = function
      Empty ->
        (Empty, false, Empty)
    | Node (l, v, r, _) ->
        let c = cmp x v in
        if c = 0 then (l, true, r)
        else if c < 0 then
          let (ll, pres, rl) = loop x l in (ll, pres, join rl v r)
        else
          let (lr, pres, rr) = loop x r in (join l v lr, pres, rr)
  in
  let setl, pres, setr = loop x set in
  setl, pres, setr

let add2 x set =
  let lb = try find_left_bound set x with
  | exn -> x
  in
  let rb = try find_right_bound set x with
  | exn -> x
  in
  let (lt, _, rt) = split lb set in
  let (_, _, rt) = split rb rt in
  let x = (min (fst lb) (fst x), max (snd rb) (snd x)) in 
(*  let (lt, _, rt) = split x set in*)
  join lt x rt;;

let remove x set =
  let rec loop = function
    | Node (l, k, r, _) ->
        let c = cmp x k in
        if c = 0 then merge l r else
        if c < 0 then bal (loop l) k r else bal l k (loop r)
    | Empty -> Empty in
  loop set

let mem x set =
  let rec loop = function
    | Node (l, k, r, _) ->
        let c = cmp x k in
        c = 0 || loop (if c < 0 then l else r)
    | Empty -> false in
  loop set

let exists = mem

let iter f set =
  let rec loop = function
    | Empty -> ()
    | Node (l, k, r, _) -> loop l; f k; loop r in
  loop set

let fold f set acc =
  let rec loop acc = function
    | Empty -> acc
    | Node (l, k, r, _) ->
          loop (f k (loop acc l)) r in
  loop acc set

let elements set = 
  let rec loop acc = function
      Empty -> acc
    | Node(l, k, r, _) -> loop (k :: loop acc r) l in
  loop [] set



(* część testowa *)
let x = empty;;
let x = add (1, 3) x;;
let x = add (-20, -15) x;;
let x = add (50, 80) x;;
let x = add (-50, -45) x;;
let x = add (-10, -9) x;;
let x = add (20, 40) x;;
let x = add (120,121) x;;
let x = add (-100,-79) x;;
let x = add (-30,-25) x;;
let x = add (-12,-12) x;;
let x = add (-6,-3) x;;
let x = add (13,19) x;;
let x = add (42,48) x;;
let x = add (91,87) x;;
let x = add (130,133) x;;