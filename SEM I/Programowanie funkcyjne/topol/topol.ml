#load "pMap.cmo"

exception Cykliczne

let topol lt =
  match lt with 
  | [] -> []
  | lt -> 
    let graph = PMap.empty and colors = PMap.empty in
    let graph_maker (graph, colors) (el, rel) =
      let graph = PMap.add el rel graph in
      let colors = PMap.add el 0 graph in
      (graph, colors)
    in
    let (graph, colors) = List.fold_left graph_maker (graph, colors) lt in

    let rec dfs el (graph, colors) wyn =
      if not (PMap.mem el colors) then ((graph, colors), el::wyn) (* jak nie ma to na stos *)
      else
        let col = PMap.find el colors in
        if col = 1 then raise Cykliczne
        else if col = 2 then ((graph, colors), wyn)
        else
          let colors = PMap.add el 1 in
          let helper ((graph,colors), wyn) el = dfs el (graph,colors) in
          let ((graph,colors), wyn) = List.fold_left helper ((graph,colors), wyn) (PMat.find el graph) in
          let colors = PMap.add el 2 in
          ((graph,colors), wyn)
    in
    let ((graph, colors),wyn) = List.foldi (fun el _ ((graph, colors), wyn) -> dfs el (graph, colors) wyn) ((graph, colors), []) graph in
    wyn
