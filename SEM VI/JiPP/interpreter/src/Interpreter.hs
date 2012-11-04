-- Interpreter.hs - Wojciech Żółtak (wz292583)
--
-- Module for a program execution.
module Interpreter where

import Grammar.AbsGrammar
import Grammar.ErrM
import Data.Map

import RunTypes
import StaticCheck

import System.IO.Unsafe


{- DECLARATIONS EXECUTION -}

dFun :: State -> Decl -> Either String State
-- Semantic function for declarations.
--
-- Variable declaration
dFun s (DVar (VDecl (LVar (LString vname)) (TName (UString tname)))) =
  let (s', loc) = newLoc (store s) (defs s ! tname)
  in Right $ State { env = insert vname (Var loc) (env s),
                     store = s', defs = defs s, flags = flags s }
--
-- Record declaration
dFun s (DType (TName (UString tname)) atrs)
  | otherwise = Right $ State { env = env s, store = store s, defs = insert tname x (defs s),
                                flags = flags s }
  where Right x = parseVarDeclLst atrs (defs s) (TRecord empty)
--
-- Function declaration
dFun s (DFun (LVar (LString vname)) args (TName (UString tname)) i) = do
  x <- parseVarArgLst args (defs s) []
  let new_env = insert vname (Fun { fArgs = x, fEnv = env s,
                                    fRType = defs s ! tname,
                                    fBody =  i }) $ env s
  do return State { env = new_env, store = store s, defs = defs s, flags = flags s }

dBlkFun :: State -> [Decl] -> Either String State
-- Semantic function for block of declarations.
dBlkFun s [] = Right s
dBlkFun s (d:xs) = do
  s' <- dFun s d
  ret <- dBlkFun s' xs
  return ret


{- EXPRESSIONS EXECUTION -}

-- Do a mathematical operation on given values.
doMath dOp iOp (MVInt v1) (MVInt v2) = MVInt $ iOp v1 v2
doMath dOp iOp (MVDouble v1) (MVInt v2) = MVDouble $ dOp v1 (fromIntegral v2)
doMath dOp iOp (MVInt v1) (MVDouble v2) = MVDouble $ dOp (fromIntegral v1) v2
doMath dOp iOp (MVDouble v1) (MVDouble v2) = MVDouble $ dOp v1 v2

-- Do a comparison operation on given values.
doCmp dOp iOp bOp (MVInt v1) (MVInt v2) = MVBool $ iOp v1 v2
doCmp dOp iOp bOp (MVDouble v1) (MVInt v2) = MVBool $ dOp v1 (fromIntegral v2)
doCmp dOp iOp bOp (MVInt v1) (MVDouble v2) = MVBool $ dOp (fromIntegral v1) v2
doCmp dOp iOp bOp (MVDouble v1) (MVDouble v2) = MVBool $ dOp v1 v2
doCmp dOp iOp bOp (MVBool v1) (MVBool v2) = MVBool $ bOp v1 v2

mathExp s e1 e2 dOp iOp = do
-- Evaluates a binary mathematical expression.
  (s', v1) <- eFun s e1
  (s'', v2) <- eFun s' e2
  return (s'', RVal $ doMath dOp iOp (fromExpVal s'' v1) (fromExpVal s'' v2))

cmpExp s e1 e2 dOp iOp bOp = do
-- Evaluates a binary comparison expression.
  (s', v1) <- eFun s e1
  (s'', v2) <- eFun s' e2
  return (s'', RVal $ doCmp dOp iOp bOp (fromExpVal s'' v1) (fromExpVal s'' v2))

logExp s e1 e2 bOp = do
-- Evaluates a binary logical expression.
  (s', v1) <- eFun s e1
  (s'', v2) <- eFun s' e2
  let
    MVBool b1 = fromExpVal s'' v1
    MVBool b2 = fromExpVal s'' v2
  do return (s'', RVal (MVBool $ bOp b1 b2))


eFun :: State -> Exp -> Either String (State, ExpVal)
-- Semantic function for expressions.
--
-- Constant
eFun s (EVal (VInt v))        = Right (s, RVal $ MVInt v)
eFun s (EVal (VDouble v))     = Right (s, RVal $ MVDouble v)
eFun s (EVal (VBool VBTrue))  = Right (s, RVal $ MVBool True)
eFun s (EVal (VBool VBFalse)) = Right (s, RVal $ MVBool False)
--
-- Variable
eFun s (EVar (LVar (LString v))) =
  let Var loc = env s ! v
  in Right (s, LVal loc)
--
-- Dot
eFun s (EDot e1 (EVar (LVar (LString l)))) = do
  (s', LVal var) <- eFun s e1
  let MVRecord rec = store s ! var
  do return (s', LVal (rec ! l))
--
-- Call
eFun s (ECall (LVar (LString (fname))) args) = 
  let
    top = findLoc $ store s -- top of memory stack

    enrich :: (State, Env) -> (Exp, String) -> (State, Env)
    -- Enriches a state/function environment with arguments passed
    -- to the function.
    enrich (s, env) (e, k) =
      let Right (s', v) = eFun s e
      in case v of
        LVal loc -> (s', insert k (Var loc) env) -- reference
        RVal val ->                              -- value
          let (st', loc) = copyLoc (store s) val
          in (changeStore s' st', insert k (Var loc) env)

  in let fun@(Fun fa fe frt fb) = env s ! fname -- get function objects
  in let arg_pairs = Prelude.map (\((k, t), e) -> (e, k)) $ zip fa args
  in let (s', env') = foldl enrich (s, fe) arg_pairs -- pass attributes
  in let env'' = insert fname fun env' -- enable recursive calls
  in do
    s'' <- iFun (changeEnv s' env'') fb
    let Return v = flags s''
    do return (freeMem 
                (changeFlags
                  (changeEnv s'' $
               env s) None) top, v) -- restore env; free memory

--
-- Math operations
eFun s (EAdd e1 e2) = mathExp s e1 e2 (+) (+) -- add
eFun s (ESub e1 e2) = mathExp s e1 e2 (-) (-) -- subtract
eFun s (EMul e1 e2) = mathExp s e1 e2 (*) (*) -- multiply
eFun s (EDiv e1 e2) = mathExp s e1 e2 (/) (div) -- divide
eFun s (ENeg e)     = do                        -- negation
  (s', v) <- eFun s e
  case fromExpVal s' v of
    MVInt i -> Right (s', RVal $ MVInt (-i))
    MVDouble d -> Right (s', RVal $ MVDouble (-d))
--
-- Comparisions
eFun s (EEq e1 e2)  = cmpExp s e1 e2 (==) (==) (==) -- equal
eFun s (ENeq e1 e2) = cmpExp s e1 e2 (/=) (/=) (/=) -- not equal
eFun s (EGt e1 e2)  = cmpExp s e1 e2 (>) (>) (>)    -- greater than
eFun s (ELt e1 e2)  = cmpExp s e1 e2 (<) (<) (<)    -- lesser than
eFun s (EGe e1 e2)  = cmpExp s e1 e2 (>=) (>=) (>=) -- greater/equal
eFun s (ELe e1 e2)  = cmpExp s e1 e2 (<=) (<=) (<=) -- lesser/equal
--
-- Logical
eFun s (EAnd e1 e2) = logExp s e1 e2 (&&) -- and
eFun s (EOr e1 e2)  = logExp s e1 e2 (||) -- or
eFun s (ENot e)     = do
  (s', v) <- eFun s e
  let MVBool b = fromExpVal s' v
  do return (s', RVal $ MVBool (not b))


{- INSTRUCTIONS EXECUTION -}

assign :: State -> Integer -> ExpVal -> State
-- Assignes an expression value to a variable stored under a given location.
assign s var (RVal (MVInt val)) = changeStore s $ insert var (MVInt val) (store s)
assign s var (RVal (MVDouble val)) = changeStore s $ insert var (MVDouble val) (store s)   
assign s var (RVal (MVBool val)) = changeStore s $ insert var (MVBool val) (store s)
assign s var (RVal (RVRecord rec)) =
  let st = store s
  in let MVRecord mrec = st ! var
  in foldl (\acc (k, v) -> assign acc (mrec ! k) (RVal $ rec ! k)) s $ assocs $ fromMVRecord (st ! var)
-- lvalues:
assign s var (LVal loc) =
  case store s ! loc of
    MVRecord rec ->
      let attrs = assocs rec
      in foldl (\acc (k, v) -> assign acc (fromMVRecord (store s ! var) ! k) (LVal v)) s attrs
    val -> assign s var (RVal val)


iFun :: State -> Instr -> Either String State
-- Semantic function for instructions
--
-- Assignment
iFun s (IAsg lab e) = do
    (s', val) <- eFun s e
    (s'', LVal var) <- eFun s lab
    return $ assign s'' var val
--
-- Declaration-instruction block
iFun s (IDecBlk (DBlk d) i) =
  let top = findLoc $ store s
  in do
    s' <- dBlkFun s d
    s'' <- iFun s' i
    return $ freeMem s'' top
--
-- Instruction block
iFun s (IBlk i) =
  let
    chk :: State -> [Instr] -> Either String State
    chk s []     = Right s
    chk s (i:xs) = do
      s' <- iFun s i
      case flags s' of
        Return x -> return s' -- skip the rest
        Break -> return s' -- skip the rest
        Continue -> return s' -- skip the rest
        _ -> chk s' xs
  in chk s i
--
-- Naked expression
iFun s (IExp e) = do
  (s', v) <- eFun s e
  return s'
--
-- Return
iFun s (IRet e) = do
  (s', v) <- eFun s e
  case v of
    RVal val -> return $ changeFlags s' (Return v)
    lv@(LVal i) -> return $ changeFlags s' (Return $ toRVal s' lv)
--
-- Break
iFun s (IBrk) = do
  return $ changeFlags s (Break)
--
-- Continue
iFun s (ICont) = do
  return $ changeFlags s (Continue)
--
-- If
iFun s (IIf e i) = do
  (s', v) <- eFun s e
  let MVBool b = fromExpVal s' v
  do if b then iFun s' i
          else Right s'
--
-- If-else
iFun s (IIfEl e i1 i2) = do
  (s', v) <- eFun s e
  let MVBool b = fromExpVal s' v
  do if b then iFun s' i1
          else iFun s' i2
--
-- While
iFun s (IWh e i) = do
  (s', v) <- eFun s e
  let MVBool b = fromExpVal s' v
  do if b then do
       s'' <- iFun s i
       case flags s'' of
         Return x -> return s''
         Break -> return $ changeFlags s'' None
         Continue -> iFun (changeFlags s'' None) (IWh e i)
         _ -> iFun s'' (IWh e i)
     else Right s'
--
-- Print
iFun s (IPrint e) = do
  (s', v) <- eFun s e
  return $ seq (unsafePerformIO $ putStrLn $ show $ toRVal s' v ) s'
