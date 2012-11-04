-- MyArray.hs - Wojciech Żółtak 292583
-- 
module MyArray(Ix(..), listArray, (!), elems, array, (//), Array) where
import MyIx

-- ------------------------------
-- Errors

_err_bounds = "Index out of bounds"
_err_undefined_elt = "Undefined element"
_err_undefined_arr = "Table has undefined elements"

-- ------------------------------
-- AVL Tree

data AVLTree i e = Leaf | Node (AVLTree i e) (i, e, Int) (AVLTree i e) deriving (Show)
-- A type which represents an AVLTree based map, indexed with `i` type
-- and values of `e` type. Node also contains an `Int` which is the height
-- of a subtree.
--
-- Only insertions are supported.

height :: AVLTree i e -> Int
-- Returns a height of a given AVLTree.
height Leaf = 0
height (Node _ (_, _, h) _) = h

leftRotation :: AVLTree i e -> AVLTree i e
-- Returns a tree obtained by performing a left rotation on a given tree.
leftRotation (Node a (i1, e1, h1) (Node b (i2, e2, h2)  c)) =
  let
    h1' = max (height a) (height b) + 1
    h2' = max h1' (height c) + 1
  in Node (Node a (i1, e1, h1') b) (i2, e2, h2') c

rightRotation :: AVLTree i e -> AVLTree i e
-- Returns a tree obtained by performing a right rotation on a given tree.
rightRotation (Node (Node a (i1, e1, h1) b) (i2, e2, h2) c) =
  let
    h2' = max (height b) (height c) + 1
    h1' = max (height a) h2' + 1
  in Node a (i1, e1, h1') (Node b (i2, e2, h2') c)

insert :: Ord i => AVLTree i e -> (i, e) -> AVLTree i e
-- Returns a tree containing elements from given tree and a element
-- from given pair under an index from that pair. If that index was already
-- used in the given tree, resulting tree contains value from the given pair.
insert Leaf (i, e) = Node Leaf (i, e, 1) Leaf
insert (Node l (ix, v, h) r) (i, e)
  | i == ix = Node l (ix, e, h) r
  | i < ix = balance $ setHeight (Node (insert l (i, e)) (ix, v, h) r)
  | i > ix = balance $ setHeight (Node l (ix, v, h) (insert r (i, e)))

bfactor :: AVLTree i e -> Int
-- Returns a balance factor of a given tree. That is - a difference between
-- height of a left subtree and a right subtree.
bfactor Leaf = 0
bfactor (Node l v r) = (height l) - (height r)

setHeight :: AVLTree i e -> AVLTree i e
-- Returns a copy of given tree with a recounted height in a root.
-- Assumes that heights of subtrees are correct.
setHeight Leaf = Leaf
setHeight (Node l (ix, v, h) r) =
  let
    h' = max (height l) (height r) + 1
  in Node l (ix, v, h') r

balance :: AVLTree i e -> AVLTree i e
-- Returns a tree with balanced root equivalent to a given tree.
-- The balance factor of the given tree must be in the range of [-2..2]
balance Leaf = Leaf
balance (Node l (ix, v, h) r)
  | fac == -2 =
    if bfactor r == -1
      then leftRotation $ Node l (ix, v, h) r
      else leftRotation $ setHeight $ Node l (ix, v, h) (rightRotation r)
  | fac == 2 =
    if bfactor l == 1
      then rightRotation $ Node l (ix, v, h) r
      else rightRotation $ setHeight $ Node (leftRotation l) (ix, v, h) r
  | otherwise = setHeight $ Node l (ix, v, h) r
  where fac = bfactor $ Node l (ix, v, h) r

treeElems :: AVLTree i e -> [e]
-- Returns a list of values contained in a given tree in an order of
-- ascending, corresponding indices.
treeElems Leaf = []
treeElems (Node l (_, e, _ ) r) = (treeElems l) ++ [e] ++ (treeElems r)

find :: Ord i => AVLTree i e -> i -> e
-- Returns a value located under a given index. Raises an error
-- if there's no such index in a given tree.
find Leaf i = error _err_undefined_elt
find (Node l (ix, v, _) r) i
  | ix == i = v
  | i < ix = find l i
  | i > ix = find r i

emptyTree :: AVLTree i e
-- Returns an empty tree.
emptyTree = Leaf


-- ------------------------------
-- Array

data Array i e = ArrayCon (AVLTree i e) (i, i) deriving (Show)
-- A type which represents an associative array index by an `i` type
-- and with values of an `e` type.
--
-- Array may have indices only from given bounds. Not every element in array
-- must be initialized. In such situation functions `(!)`, `elems` and `(//)`
-- fails.

listArray :: Ix i => (i, i) -> [e] -> Array i e
-- Returns an array with indices in given bounds and elements from a given list.
-- Values are associated with consecutive indices, starting from lower bound.
-- If a length of an array jest greater then a length of a range, the array will
-- be cut (after a n-th element, where n is the range size) to fit the range.
listArray ix xs =
  array ix $ zip (range ix) xs

(!) :: Ix i => Array i e -> i -> e
-- Returns a value stored in a given array under a given index.
(!) (ArrayCon tree ix) i
  | inRange ix i = find tree i
  | otherwise = error _err_bounds

elems :: Ix i => Array i e -> [e]
-- Returns a list of values stored in a given array.
elems (ArrayCon tree ix)
  | length elts == rangeSize ix = elts
  | otherwise = error _err_undefined_arr
  where elts = treeElems tree
  
array :: Ix i => (i, i) -> [(i, e)] -> Array i e
-- Returns an array with indices in given bounds and elements from a given list.
-- The list should contain pairs in the form of `(index, value)`.
array ix els = (ArrayCon emptyTree ix) // els

(//) :: Ix i => Array i e -> [(i, e)] -> Array i e
-- Returns an array which is a copy of given array updated with elements from
-- a given list. The list should contain paris in the form of `(index, value)`.
(//) (ArrayCon tree ix) els =
  let
    fun (i, e) tree
      | inRange ix i = insert tree (i, e)
      | otherwise = error _err_bounds
  in ArrayCon (foldr fun tree els) ix
