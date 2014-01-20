-- main.hs - Wojciech Żółtak 292583
-- 
import Graph

findBounds :: [[Int]] -> (Int, Int) -> (Int, Int)
-- Returns a pair containing a minimal a maximal number from a given list.
findBounds [] (mi, ma) = (min mi 1, ma)
findBounds ([]:xs) a = findBounds xs a
findBounds ((x:xs):ys) (mi, ma) = findBounds (xs:ys) ((min mi x), (max ma x))

blankGraph :: (Int, Int) -> Graph
-- Returns a graph with nodes `[s..e]` and no edges.
blankGraph (s, e) =
  graph (s, e) (map (\x -> (x, [])) [s..e])

buildGraph :: String -> Graph
-- Builds a graph from a given string.
buildGraph input =
  let
    helper (x:xs) = (x, xs)
    ln = lines input
    wd = map words ln
    nums = map (map (read :: String -> Int)) wd
    sb = head $ head nums
    bounds = findBounds nums (sb, sb)
  in insertNodes (blankGraph bounds) (map helper nums)

output :: [Int] -> String
-- Returns an output string
output [] = "\n"
output (x:xs) = (show x) ++ " " ++ output xs

main =
-- Reads a graph from an input and prints out a list of nodes accessible
-- from a node number 1.
  interact (\s -> output $ accessible (buildGraph s) 1)
