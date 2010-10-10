(** Punkt na płaszczyźnie *)
type point = float*float;;


(** Poskładana kartka: ile razy kartkę przebije szpilka wbita w danym
punkcie *)
type kartka = point -> int;;


(** [odbij p p1 p2] zwraca punkt na płaszczyźnie,
będący obrazem punktu [p] w symetrii względem prostej wyznaczonej
przez punkty [p1 p2]

korzystamy z wyprowadzonego ręcznie wzoru na liczbach zespolonych:  
[p' = (p2^2/p) * (|p|^2/|p2|^2)]
dla [p1 = (0., 0.)]

na płaszczyźnie zespolonej :
przesuwamy układ współrzędnych o [-p1]
potraktujmy [p2] i [p] jako wektory wodzące
obracamy p o rządany kąt wymnażając go razy [p2^2/p]
dopasowujemy jego długość mnożąc razy [|p|^2/|p2|^2]
przesuwamy układ z powrotem o [+p1]
*)
let odbij (pktx, pkty) (p1x, p1y) (p2x, p2y) = 
  (* przesuniecie o -p1 *)
  let p2x = p2x -. p1x in
  let p2y = p2y -. p1y in
  let pktx = pktx -. p1x in
  let pkty = pkty -. p1y in
  (* obliczenie p' *)
  let x = pktx *. ( p2x *. p2x -. p2y *. p2y ) +. 2. *. p2x *. p2y *. pkty in
  let x = x /. ( p2x *. p2x +. p2y *. p2y ) in
  let y = pkty *. ( p2y *. p2y -. p2x *. p2x ) +. 2. *. p2x *. p2y *. pktx in
  let y = y /. ( p2x *. p2x +. p2y *. p2y ) in
  (* przesuniecie o p1 *)
  let x = x +. p1x in
  let y = y +. p1y in
  ((x, y):point);;


(** [prostokat p1 p2] zwraca kartkę, reprezentującą domknięty prostokąt
o bokach równoległych do osi układu współrzędnych i lewym dolnym rogu [p1]
a prawym górnym [p2]. Punkt [p1] musi więc być nieostro na lewo i w dół
od punktu [p2]. Gdy w kartkę tę wbije się szpilkę wewnątrz
(lub na krawędziach) prostokąta, kartka zostanie przebita 1 raz,
w pozostałych przypadkach 0 razy

punkt należy do kartki, gdy jego odpowiednie współrzędne mieszczą się
w przedziałach wyznaczonych, przez rzutowanie punktów [p1] oraz [p2]
na osie układu współrzędnych *)
let prostokat (p1x, p1y) (p2x, p2y) =
  let f (pktx, pkty) =
    if p1x <= pktx && pktx <= p2x && p1y <= pkty && pkty <= p2y then 1
    else 0
  in
    if p1x > p2x || p1y > p2y then failwith "Zle wymiary kartki"
    else (f:kartka);;


(** [kolko p r] zwraca kartkę, reprezentującą kółko domknięte o środku
w punkcie [p] i promieniu [r]

punkt należy do kartki, gdy nie leży dalej niż [r] od jej środka *)
let kolko (p1x, p1y) r = 
  let f (pktx, pkty) =
    let x = pktx -. p1x in
    let y = pkty -. p1y in
    if x *. x +. y *. y <= r *. r then 1
    else 0
  in (f:kartka);;


(** [iloczyn_wektorowy p1 p2 p3] zwraca współrzędną [z] wektora
będącego wynikiem iloczynu wektorowego wektorów [p1p2] oraz [p1p3]

jego znak mówi o tym, z której strony prostej [p1p2] leży punkt [p3]
(patrząc w kierunku od [p1] do [p2]):
  dodatni - po lewej
  ujemny - po prawej *)
let iloczyn_wektorowy (p1x, p1y) (p2x, p2y) (p3x, p3y) = 
  let w1x = p2x -. p1x and w1y = p2y -. p1y in
  let w2x = p3x -. p1x and w2y = p3y -. p1y in
  w1x *. w2y -. w1y *. w2x;;


(** [zloz p1 p2 k] składa kartkę [k] wzdłuż prostej przechodzącej przez
punkty [p1] i [p2] (muszą to być różne punkty). Papier jest składany
w ten sposób, że z prawej strony prostej (patrząc w kierunku od [p1] do [p2])
jest przekładany na lewą. Wynikiem funkcji jest złożona kartka. Jej
przebicie po prawej stronie prostej powinno więc zwrócić 0.
Przebicie dokładnie na prostej powinno zwrócić tyle samo,
co przebicie kartki przed złożeniem. Po stronie lewej -
tyle co przed złożeniem plus przebicie rozłożonej kartki w punkcie,
który nałożył się na punkt przebicia.

do sprawdzania po której stronie prostej znajduje się punkt
wykorzystam własności iloczynu wektorowego *)
let zloz pr1 pr2 (kartka:kartka) =
  let nowa_kartka p1 =
    let il = iloczyn_wektorowy pr1 pr2 p1 in
    if il = 0. then kartka p1 (* trafienie w zgiecie *)
    else if il < 0. then 0 (* totalne pudlo *)
    else kartka p1 + kartka (odbij p1 pr1 pr2 )  (* potencjalne trafienia *)
  in (nowa_kartka:kartka);;


(** [skladaj [(p1_1,p2_1);...;(p1_n,p2_n)] k =
zloz p1_n p2_n (zloz ... (zloz p1_1 p2_1 k)...)]
czyli wynikiem jest złożenie kartki [k] kolejno wzdłuż wszystkich prostych
z listy *) 
let skladaj lista kartka =
  let f k (p1, p2) = 
    zloz p1 p2 k
  in List.fold_left f kartka lista;;
