-- RunTypes.hs - Wojciech Żółtak (wz292583)
-- 
-- Module contains a data types used by an interpreter with some useful
-- manipulation functions.
module RunTypes  where

import Grammar.AbsGrammar
import Grammar.ErrM
import Data.Map


{- TYPES -}

-- Algebraic type for an environment object.
-- 
-- Contains both objects used during static type check and
-- code execution.
data Obj   = -- Variable in memory; contains store address
             Var Integer |
             -- Function object
             Fun { fArgs  :: [(String, DType)], -- arguments
                   fEnv   :: Env, -- environment
                   fRType :: DType, -- return type
                   fBody  :: Instr } | -- body
             -- Data type object
             VarType DType deriving Show

-- Environment map. Map labels to objects.
type Env   = Map String Obj
-- Store map. Map integers ("memory" locations) to actual values;
type Store = Map Integer MemVal

-- Algebraic type for values data type representation.
data DType = TNone | TInt | TBool | TDouble |
             TRecord (Map String DType) deriving (Ord, Eq, Show)
-- Type definitions map. Map type names to data type objects.
type Defs  = Map String DType

-- Algebraic type for an actual values in memory.
--
-- MVRecord is a record data value, which map its attributes to
-- other locations in memory (it's necessary when we pass an attribute
-- by refernce).
--
-- RVRecord is a "temporary" record data value (`rvalue`), used when
-- record is returned by a function (we don't want to allocate a place
-- in store for it, because freeing it later may produce "holes" in
-- store indexing).
data MemVal = MVInt Integer | MVBool Bool | MVDouble Double |
              MVRecord (Map String Integer) |
              RVRecord (Map String MemVal) deriving Show

-- Algebraic type for an expression value.
--
-- LVal is a `lvalue` - a reference to an object in a store.
--
-- RVal is a `rvalue` - a temporary object produced during the expression
-- evaluation.
data ExpVal = LVal Integer | RVal MemVal deriving Show

-- Algebraic type for flow exection control - like returning values,
-- loop breaks etc.
data Flow = None | Return ExpVal | Break | Continue deriving Show

-- Algebraic type for a program state representation during execution.
data State = State { env   :: Env,   -- environment (functions and value locations)
                     store :: Store, -- store
                     defs  :: Defs,  -- types definitions
                     flags :: Flow   -- flow control
                   } deriving Show
-- Algebraic type for a program state representation during type checking.
data TState = TState { tEnv  :: Env,    -- environment (functions and data types)
                       tDefs :: Defs,   -- types definitions
                       tRType :: DType, -- expected return type
                       tWhile :: Bool   -- true if in while block
                     } deriving Show


{- FUNCTIONS -}

defDecls :: Defs
-- Returns an basic types definitions.
defDecls =
  fromList [("BOOL", TBool), ("DOUBLE", TDouble), ("INTEGER", TInt)]

defValue :: DType -> MemVal
-- Returns a default value of a basic type;
defValue TInt = MVInt 0
defValue TDouble = MVDouble 0.0
defValue TBool = MVBool False

findLoc :: Store -> Integer
-- Returns an empty location in a given store.
findLoc s = if size s == 0 then 0
                           else fst (findMax s) + 1

copyLoc :: Store -> MemVal -> (Store, Integer)
-- Allocates a copy of a given basic-type value in a given store.
copyLoc s v =
  let loc = findLoc s
  in (insert loc v s, loc)


newLoc :: Store -> DType -> (Store, Integer)
-- Allocates a place in a given store for a given type.
newLoc s (TRecord rec) = -- records
  let loc = findLoc s
      alloc (s, lst) (k, v) =
        let (s', loc) = newLoc s v
        in (s', (k, loc):lst)
  in let s' = insert loc (MVInt 0) s  -- temporary value
  in let (s'', attrs) = foldl alloc (s', []) $ assocs rec
  in (insert loc (MVRecord $ fromList attrs) s'', loc)
newLoc s t = -- basic types
  let loc = findLoc s
  in (insert loc (defValue t) s, loc)

freeMem :: State -> Integer -> State
-- Removes values from store which locations are (>=) than argument
freeMem s i =
  let st = store s
  in let max = findLoc st
  in changeStore s $ foldl (\st i -> delete i st) st [i..max]

fromMVRecord :: MemVal -> Map String Integer
-- Extracts an attributes map from MVRecord.
fromMVRecord (MVRecord rec) = rec

fromExpVal :: State -> ExpVal -> MemVal
-- Extracts value of a given expression value. Returns a "temporary"
-- value (RVRecord in record case).
fromExpVal s (RVal mv) = mv
fromExpVal s lv@(LVal i) = fromExpVal s $ toRVal s lv

getTypeObj :: Obj -> DType
-- Extracts a variable type from an object type.
getTypeObj (VarType t) = t


toRVal :: State -> ExpVal -> ExpVal
-- Transforms any expression value into a `rvalue` expression value.
toRVal s (LVal i) =
  case store s ! i of
    MVRecord rec ->
      let attrs = Prelude.map (\(k, i) -> (k, toRVal s $ LVal i)) $ assocs rec
      in let attrs' = Prelude.map (\(k, RVal v) -> (k, v)) attrs
      in RVal $ RVRecord $ fromList attrs'
    x -> RVal x
toRVal s rv@(RVal v) = rv

changeEnv :: State -> Env -> State
-- Overwrites envrionment in a given state.
changeEnv s e = State { env = e, store = store s, defs = defs s,
                        flags = flags s }

changeFlags :: State -> Flow -> State
-- Overwrites flags in a given state.
changeFlags s f = State { env = env s, store = store s, defs = defs s,
                          flags = f }

changeStore :: State -> Store -> State
-- Overwrites store in a given state.
changeStore s v = State { env = env s, store = v, defs = defs s,
                          flags = flags s }

setWhile :: TState -> TState
-- Sets a while flag in a given TState
setWhile s = TState { tEnv = tEnv s, tDefs = tDefs s,
                      tRType = tRType s, tWhile = True }

unsetWhile :: TState -> TState
-- Sets a while flag in a given TState
unsetWhile s = TState { tEnv = tEnv s, tDefs = tDefs s,
                        tRType = tRType s, tWhile = False }

