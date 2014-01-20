-- Graph.hs - Wojciech Żółtak 292583
-- 
module Graph(Graph, graph, insertNodes, adj, accessible) where
import MyArray

data Graph = Graph { edges :: Array Int [Int]
                   , bounds :: (Int, Int) } deriving (Show)
-- A type which represents a directed graph labeled by a type `e` and with
-- associated values of a type `v`.

graph :: (Int, Int) -> [(Int, [Int])] -> Graph
-- Returns a graph which can contain a nodes with labels in given bounds
-- and initially contains a nodes from a given list. The list must contain
-- triplets in the form of `(label, value, list_of_adjacent_labels)`.
graph ix elts =
  let
    g = Graph { edges = array ix []
              , bounds = ix }
  in insertNodes g elts

insertNodes :: Graph -> [(Int, [Int])] -> Graph
-- Returns a graph builded from a given graph by insertion of given nodes.
-- Inserted nodes may screen nodes from the given graph. The list must contain
-- triplets in the form of `(label, value, list_of_adjacent_labels)`.
insertNodes g elts = Graph { edges = edges g // elts
                           , bounds = bounds g }

adj :: Graph -> Int -> [Int]
-- Returns a list of an adjacent nodes of a given node in a given graph.
adj g e = edges g ! e

accessible :: Graph -> Int -> [Int]
-- Returns a list of nodes accessible from a given node in a given graph.
accessible g e =
  let
    -- An array with node's "colors". 0 for unvisited, 1 for visited. 
    colors = listArray (bounds g) [ 0 | x <- range (bounds g) ]
    bfs [] c a = reverse a
    bfs (x:xs) c a  =
      let
        c' = c // [(x, 1)]
      in case c ! x of
        1 -> bfs xs c' a
        0 -> bfs (xs ++ (g `adj` x)) c' (x:a)
  in bfs (g `adj` e) colors []
