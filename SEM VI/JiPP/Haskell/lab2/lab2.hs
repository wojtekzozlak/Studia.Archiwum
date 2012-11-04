-- Zad1

incAll :: [[Int]] -> [[Int]]
incAll xs = map (map (+1)) xs

power n = foldr (*) 1 [1..n]

nconcat :: [[a]] -> [a]
nconcat xs = foldr (++) [] xs


-- Zad2

data Tree a = Empty | Node a (Tree a) (Tree a) deriving (Eq, Ord)

instance Show a => Show (Tree a) where
  show Empty = "_"
  show (Node b l r) = "[" ++ show l ++ ", " ++ show b ++ ", " ++ show r ++ "]"

instance Functor Tree where
    fmap f Empty = Empty
    fmap f (Node b l r) = Node (f b) (fmap f l) (fmap f r)

-- wydajniej byloby z akumulatorem
toList :: Tree a -> [a]
toList Empty = []
toList (Node a l r) = [a] ++ toList l ++ toList r

insert :: (Ord a) => a -> Tree a -> Tree a
insert e Empty = (Node e Empty Empty)
insert e (Node v l r)
    | v == e = (Node v l r)
    | v > e = (Node v (insert e l) r)
    | otherwise = (Node v l (insert e r))

contains :: (Ord a) => a -> Tree a -> Bool
contains e Empty = False
contains e (Node v l r)
    | v == e = True
    | v > e = contains e l
    | otherwise = contains e r

fromList :: (Ord a) => [a] -> Tree a
fromList xs = foldr (insert) Empty xs

-- Zad3


-- TODO : dlaczeeeego tak?
instance Functor (Either e) where
  fmap f (Left l) = Left l
  fmap f (Right r) = Right (f r)

reverseRight1 :: Either e [a] -> Either e [a]
reverseRight1 xs = fmap reverse xs

reverseRight2 :: Either e [a] -> Either e [a]
reverseRight2 (Left l) = Left l
reverseRight2 (Right r) = Right (reverse r)

