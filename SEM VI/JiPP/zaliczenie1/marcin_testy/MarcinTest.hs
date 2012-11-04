--
-- Copyright 2012 Osowski Marcin
--
--
-- Put your Array module in MyArray, and Ix module in MyIx, or change
-- the imports below.
--

import MyArray

import Test.QuickCheck
import System.IO.Unsafe
import Data.List
import qualified Data.Ix
import qualified Data.Array


cmp :: (Eq b) => (a -> b) -> (a -> b) -> a -> Bool
cmp f g x = (f x) == (g x)


showDebugs = False

debugDump :: String -> a -> a
debugDump s
    | showDebugs        = seq (unsafePerformIO $ putStrLn s)
    | otherwise         = id

--
-- Testing MyIndex
--

-- range testing against library--
propRangeChar :: (Char, Char) -> Bool
propRangeChar = cmp range Data.Ix.range

propRangeInteger :: (Integer, Integer) -> Bool
propRangeInteger = cmp range Data.Ix.range

propRangeInt :: (Int, Int) -> Bool
propRangeInt = cmp range Data.Ix.range

propRangeIntPair :: ((Int, Int), (Int, Int)) -> Bool
propRangeIntPair = cmp range Data.Ix.range

-- index testing against library
propIndexIntPair :: ((Int, Int), (Int, Int), (Int, Int)) -> Bool
propIndexIntPair (lo, hi, i)
    | elem i $ range (lo, hi)  = index (lo, hi) i == Data.Ix.index (lo, hi) i
    | otherwise                = True --(bad test data)

-- inRange testing against library
propInRangeIntPair :: ((Int, Int), (Int, Int), (Int, Int)) -> Bool
propInRangeIntPair (lo, hi, i) = inRange (lo, hi) i == Data.Ix.inRange (lo, hi) i

-- rangeSize should be equal to length . range
propRangeSize :: (Ix t) => t -> (t, t) -> Bool
propRangeSize typeEnforcer = cmp rangeSize (length . range)

--
-- Testing MyArray
--

-- Creating an array and decomposing it back to list should be an identity --
propListArray :: [Char] -> Bool
propListArray es =
    elems (listArray (1,length es) es) == es

-- Testing (!)
propListAccess :: [Char] -> Int -> Bool
propListAccess s n
    | (0 <= n) && (n < s_len)    = ((listArray (0, s_len - 1) s) ! n) == (s !! n)
    | otherwise                  = True
    where
        s_len = length s


sizedArgs :: Int -> Args
sizedArgs n = Args {
            replay = Nothing,
            maxSuccess = 200,
            maxDiscard = 1,
            maxSize = n
    }

main = do

    let stringArgs = sizedArgs 80
    let stringIntArgs = sizedArgs 30
    let charArgs = sizedArgs 80
    let intArgs = sizedArgs 30
    let integerArgs = sizedArgs 80
    let intPairArgs = sizedArgs 20

    -- MyIndex
    quickCheckWith charArgs    $ propRangeChar
    quickCheckWith intArgs     $ propRangeInt
    quickCheckWith integerArgs $ propRangeInteger
    quickCheckWith intPairArgs $ propRangeIntPair

    quickCheckWith intPairArgs $ propIndexIntPair

    quickCheckWith intPairArgs $ propInRangeIntPair

    quickCheckWith charArgs    $ propRangeSize '\0'
    quickCheckWith intArgs     $ propRangeSize (0 :: Int)
    quickCheckWith integerArgs $ propRangeSize (0 :: Integer)
    quickCheckWith intPairArgs $ propRangeSize ((0,0) :: (Int, Int))

    -- MyArray
    quickCheckWith stringArgs    $ propListArray
    quickCheckWith stringIntArgs $ propListAccess

