import Data.List

my_head (x:xs) = x
my_tail (x:xs) = xs

append [] y = y
append (x:xs) y = x:(append xs y)

my_take n [] = []
my_take 0 x = []
my_take n (x:xs) = x:(my_take (n - 1) xs)

my_drop n [] = []
my_drop 0 x = x
my_drop n (x:xs) = my_drop (n - 1) xs

inits x =
  let
    l = length(x)
    f n = take n x
  in map f [0..l]

partitions x =
  let
    l = length(x)
  in [ (take n x, drop n x) | n <- [0..l] ]

my_permutations [] = [[]]
my_permutations xs =  [x:ys | x <- xs, ys <- my_permutations (delete x xs) ]

nub_helper [] a = []
nub_helper (x:xs) a | elem x a = nub_helper xs a
                    | otherwise = x:(nub_helper xs $ x:a)

my_nub x = nub_helper x []

nwd a b | b == 0 = a
        | a >= b = nwd b (a - b)
        | otherwise = nwd b a

triads n =
  let
    t = [ (x, y, z) | x <- [1..n], y <- [x..n], z <- [y..n] ]
    pyth (x, y, z) = x^2 + y^2 == z^2
    simplifier (x, y, z) = 
      let
        d = nwd x (nwd y z)
      in (x `div` d, y `div` d, z `div` d)
  in nub $ map simplifier (filter pyth t)


indexOf :: Char -> String -> Maybe Int
indexOf_helper x [] k = Nothing
indexOf_helper e (x:xs) k
  | x == e = Just k
  | otherwise = indexOf_helper e xs $ k + 1
indexOf e x = indexOf_helper e x 0

positions :: Char -> String -> [Int]
positions_helper e [] k a = a
positions_helper e (x:xs) k a 
  | e == x = positions_helper e xs (k + 1) (k:a)
  | otherwise = positions_helper e xs (k + 1) $ a
positions e x = reverse $ positions_helper e x 0 []


reverse_helper [] a = a
reverse_helper (x:xs) a = reverse_helper xs (x:a)
my_reverse x = reverse_helper x []


smain :: String -> String
main = interact smain

smain input = "Hello world\n"
