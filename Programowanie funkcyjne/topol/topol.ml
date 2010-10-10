(* wyjatek rzucany przez [topol] gdy zaleznosci sa cykliczne *)
exception Cykliczne

(*  [graph_maker (map1, map2) (el, rel)] dodaje do [map1] 
    listę rel oznaczającą krawędzie skierowane na wierzchołki
    znajdujące się na liście, wychodzące z [el], a do [map2]
    pustą flagę odwiedzin niezbędną do DFSa 
    
    złożoność czasowa
      Teta[log n] względem rozmiaru mapy
    złożoność pamięciowa
      Teta[n] względem długości [rel] *)
let graph_maker (graph, colors) (el, rel) =
  let graph = PMap.add el rel graph in
  let colors = PMap.add el 0 colors in
  (graph, colors)

(*  [dfs ((map1, map2), wyn) el] przeszukuje w głąb graf reprezentowany przez 
    [map1], począwszy od [el], korzystając z [map2] jako mapy flag odwiedzin
    wierzchołków. W momencie opuszczenia wierzchołka zostaje on wrzucony na
    stos [wyn].

    Ponieważ w grafie znajdują się tylko wierzchołki grafu posiadające sąsiadów
    najpierw sprawdza istnienie wierzchołka w mapie i w razie czego od razu
    wrzuca na stos (taki wierzchołek nie posiada sąsiadów) i zaznacza to
    uzupełniając mapę flag.

    Dla pozostałych funcja sprawdza flagę wierzchołka
    - dla [1] (odwiedzonego, ale nie wrzuconego na stos) rzuca wyjątkiem
    (idąc do miejsca w którym się znaleźliśmy wyszliśmy z tego wierzchołka,
    więc znaleziono cykl)
    - dla [2] (wierzchołek wrzucony na stos) idziemy dalej, bo zrobiliśmy dla
    niego już wszystko co można było
    - dla [0] (wierzchołek nieodwiedzony) zaznaczamy wejście w mapie flag,
    dokonujemy DFSa dla wszystkich sąsiadów i o ile po drodze nic się nie
    zepsuło (np. rzucono wyjątek) wrzucamy wierzchołek na stos, zaznaczamy
    flagę i wychodzimy poziom wyżej w rekurencji 
    
    złożoność czasowa
      O[E( log V ) + V (log V) ] względem ilości krawędzi
    i wierzchołków w grafie
    
    złożoność pamięciowa
      O[E + V] względem jak wyżej *)
let rec dfs ((graph, colors), wyn) el =
  if not (PMap.mem el colors) then
    let colors = PMap.add el 2 colors in
    ((graph, colors), el::wyn)
  else
    let col = PMap.find el colors in
    if col = 1 then raise Cykliczne
    else if col = 2 then ((graph, colors), wyn)
    else
      let colors = PMap.add el 1 colors in 
      let neighbours = (PMap.find el graph) in
      let acc = ((graph,colors), wyn) in
      let ((graph,colors), wyn) = List.fold_left dfs acc neighbours in
      let colors = PMap.add el 2 colors in 
      ((graph,colors), el::wyn)


(*  [topol (a_1,[a_11;...;(a_m,[a_m1;...;a_mk])]
    Dla danej listy [(a_1,[a_11;...;(a_m,[a_m1;...;a_mk])]
    zwraca liste, na ktorej kazdy z elementow a_i oraz a_ij
    wystepuje dokladnie raz i ktora jest uporzadkowana w taki sposob,
    ze kazdy element a_i jest przed kazdym z elementow a_i1 ... a_il.
    Mozna zalozyc, ze elementy a_1 ... a_m nie powtarzaja sie jak
    rowniez ze kazda z list [a_11;...;a_1n] ... [a_m1;...;a_mk] ma
    niepowtarzajace sie elementy
    
    Funkcja najpierw buduje dwie mapy potrzebne do skorzystania z DFSa,
    a następnie odpala go po kolei na wszystkich wierzchołkach, które
    posiadają jakichś sąsiadów. Pojedynczy nie wystarczyłby, ze względu
    możliwość istnienia paru rozłącznych grafów. DFS zbiera na stos
    wierzchołki w momencie wychodzenia z nich tworząc w ten sposób
    listę wynikową.

    A dlaczego miałoby to działać?
    To co tak naprawdę chcemy zrobić przy sortowaniu topologicznym to
    "wyjmowanie" z grafu wierzchołków, z których nie wychodzą żadne
    "strzałki" i układanie ich na stosie (bo na pewno jest to fragment
    rozwiązania, o ile takie istnieje). W ten sposób usuwamy też część
    krawędzi i możemy operację powtórzyć. Do wyczerpania zapasów.
    I właśnie coś takiego robi DFS - w momencie gdy opuszcza wierzchołek
    wrzuca go na stos i "usuwa" z grafu. Wierzchołek jest zaś opuszczony
    dopiero wtedy, gdy zostaną opuszczeni wszyscy jego sąsiedzi, więc
    de facto nie wychodzą już z niego żadne krawędzie.

    Pozostaje kwestia cykli. Póki możemy zdejmować wierzchołki wszystko
    jest na pewno ok. Jedyny zaś moment, w którym dzieje się coś dotąd
    nie omówionego, jest gdy trafiamy na wierzchołek odwiedzony, ale nie 
    usunięty. Wtedy wiadomo, że trafiliśmy do punktu, z którego wyszliśmy
    i znaleziono cykl.
    Więcej przypadków nie napotkamy, więc dowód jest skończony.


    złożoność czasowa
    - tworzenie map O[n log n] względem długości listy
    - jedno przejście DFS O[E( log V ) + V (log V) ] względem ilości krawędzi
    i wierzchołków w pojedynczym grafie
    - foldi - niby n*DFS, ale w przypadku już przerobionych wierzchołków koszt
    DFS spada do stałego, więc całość O[E( log V ) + V (log V) ] względem
    ilości krawędzi i wierzchołków w całości danych
    Podsumowując
      O[E( log V ) + V (log V) ]

    złożoność pamięciowa
      O[E + V] względem krawędzi i wierzchołków w danych *)
let topol lt =
  match lt with 
  | [] -> []
  | lt -> 
    let graph = PMap.empty and colors = PMap.empty in
    let (graph, colors) = List.fold_left graph_maker (graph, colors) lt in
    let helper key _ acc =
      dfs acc key
    in
    let acc = ((graph, colors), []) in
    let ((graph, colors), wyn) = PMap.foldi helper graph acc in
    wyn