-- StaticCheck.hs - Wojciech Żółtak (wz292583)
--
-- Module for a program static type check.
module StaticCheck where

import Grammar.AbsGrammar
import Grammar.ErrM
import Data.Map

import RunTypes

{- DECLARATIONS CHECK -}

err_unknown_type = "unknown type: " -- unknown type error text

parseVarDeclLst :: [VarDecl] -> Defs -> DType -> Either String DType
-- Constructs a type from a list of attribute declarations.
parseVarDeclLst [] d acc = Right acc
parseVarDeclLst (VDecl (LVar (LString vname))
                (TName (UString tname)):xs) d (TRecord acc)
  | member tname d = parseVarDeclLst xs d $ TRecord (insert vname (d ! tname) acc)
  | otherwise = Left (err_unknown_type ++ tname)

parseVarArgLst :: [VarArg] -> Defs -> [(String, DType)] -> Either String [(String, DType)]
-- Builds function arguments signature from list of argument declarations.
parseVarArgLst [] _ acc = Right $ reverse acc
parseVarArgLst (VArg (LVar (LString vname)) (TName (UString tname)):xs) d acc
  | member tname d = parseVarArgLst xs d $ (vname, d ! tname):acc
  | otherwise = Left (err_unknown_type ++ tname)

parseCallArgLst :: [Exp] -> [(String, DType)] -> TState -> Either String Bool
-- Checks if a given list of expressions matches a function signature.
parseCallArgLst [] [] _ = Right True
parseCallArgLst (e:xs) ((_, t):yx) s = do
  at <- chkExp s e
  if at == t then do
                ret <- parseCallArgLst xs yx s
                return ret
             else Left $ "Expected `" ++ show(t) ++ "`, not `" ++ show(at) ++ "`"
parseCallArgLst _ _ _ = Left $ "Incorect number of arguments"


chkDecl :: TState -> Decl -> Either String TState
-- Semantic check function for declarations.
--
-- Variable declaration
chkDecl s (DVar (VDecl (LVar (LString vname)) (TName (UString tname))))
  | member tname (tDefs s) =
      Right TState { tEnv  = insert vname (VarType $ tDefs s ! tname) (tEnv s),
                     tDefs = tDefs s, tRType = tRType s, tWhile = tWhile s }
  | otherwise = Left $ err_unknown_type ++ tname
--
-- Record declaration
chkDecl s (DType (TName (UString tname)) atrs) = do
  x <- parseVarDeclLst atrs (tDefs s) (TRecord empty)
  return $ TState { tEnv  = tEnv s, tDefs = insert tname x (tDefs s),
                    tRType = tRType s, tWhile = tWhile s }
--
-- Function declaration
chkDecl s (DFun (LVar (LString vname)) args (TName (UString tname)) i) = do
  x <- parseVarArgLst args (tDefs s) []
  if member tname (tDefs s) then
    let fobj = Fun { fArgs = x, fEnv = tEnv s, fRType = tDefs s ! tname, fBody =  i }
    in let env' = insert vname fobj $ tEnv s
    in let fenv = foldl (\acc (l, t) -> insert l (VarType t) acc) env' x
    in do -- function body check
      _ <- chkInstr (TState { tEnv = fenv, tDefs = tDefs s,
                              tRType = fRType fobj, tWhile = False }) i
      return $ TState { tEnv = env', tDefs = tDefs s, tRType = tRType s,
                        tWhile = tWhile s }
    else Left $ err_unknown_type ++ tname

chkDeclBlk :: TState -> [Decl] -> Either String TState
-- Semantic check function for block of declarations.
chkDeclBlk s [] = Right s
chkDeclBlk s (d:xs) = do
  s' <- chkDecl s d
  ret <- chkDeclBlk s' xs
  return ret



{- EXPRESSIONS CHECK -}

-- Binary operation error text
binErr op t1 t2 = "`" ++ (show t1) ++ " " ++ op ++ " " ++ (show t2) ++ "` is not a valid expression"

mathResolver :: String -> DType -> DType -> Either String DType
-- Returns a resulting type of a mathematical binary operation on given types.
mathResolver _ TInt TInt       = Right TInt
mathResolver _ TDouble TInt    = Right TDouble
mathResolver _ TInt TDouble    = Right TDouble
mathResolver _ TDouble TDouble = Right TDouble
mathResolver op a b            = Left $ binErr op a b

logResolver :: String -> DType -> DType -> Either String DType
-- Returns a resulting type of a logical binary operation on given types.
logResolver _ TBool TBool = Right TBool
logResolver op a b        = Left $ binErr op a b

cmpOrdResolver :: String -> DType -> DType -> Either String DType
-- Returns a resulting type of a comparison binary operation on given ordered types.
cmpOrdResolver op a b =
  case mathResolver "" a b of -- like math...
    Right _ -> Right TBool
    Left _  -> Left $ binErr op a b -- ...but we overrides error message

cmpResolver :: String -> DType -> DType -> Either String DType
-- Returns a resulting type of a comparison binary operation on given ordered types.
-- Used in equality testing.
cmpResolver op TBool TBool = Right TBool
cmpResolver op a b = cmpOrdResolver op a b


chkBinExp :: TState -> Exp -> Exp ->
             (DType -> DType -> Either String DType) -> Either String DType
-- Checks types in a binary expression using a given resolver.
chkBinExp s e1 e2 r =
  case (chkExp s e1, chkExp s e2) of
    (Right v1, Right v2) -> r v1 v2
    (Left x, _) -> Left x
    (_, Left y) -> Left y

chkUnMathExp :: String -> TState -> Exp -> Either String DType
-- Checks type of an unary mathematical expression.
chkUnMathExp op s e =
  case chkExp s e of
    Right TInt    -> Right TInt
    Right TDouble -> Right TDouble
    Right t       -> Left $ "`" ++ op ++ " " ++ (show t) ++ "` is not a valid expression"
    err           -> err

chkUnLogExp :: String -> TState -> Exp -> Either String DType
-- Checks type of an unary logical expression.
chkUnLogExp op s e =
  case chkExp s e of
    Right TBool -> Right TBool
    Right t     -> Left $ "`" ++ op ++ " " ++ (show t) ++ "` is not a valid expression"
    err         -> err


chkExp :: TState -> Exp -> Either String DType 
-- Semantic check function for expressions.
--
-- Constant
chkExp s (EVal (VInt _))    = Right TInt
chkExp s (EVal (VDouble _)) = Right TDouble
chkExp s (EVal (VBool _))   = Right TBool
--
-- Variable
chkExp s (EVar (LVar (LString v))) =
  if member v $ tEnv s then Right $ (getTypeObj $ tEnv s ! v)
                       else Left $ "Unknown label `" ++ v ++ "`"
--
-- Dot
chkExp s (EDot e1 (EVar (LVar (LString l)))) =
  -- first must be a record type; second a good label
  case chkExp s e1 of
    Right (TRecord t) -> if member l t then Right $ t ! l
			 else Left $ "`" ++ l ++ "` is not a attribute of `" ++ show(t) ++ "`"
    Right t           -> Left $ "`" ++ show(t) ++ "` is not subscriptable"
    err               -> err
--
-- Call
chkExp s (ECall (LVar (LString (fname))) args) = 
  if member fname $ tEnv s then
    case tEnv s ! fname of
      Fun fa fe frt fb -> do
          x <- parseCallArgLst args fa s
          return frt
      _ -> Left $ "`" ++ fname ++ "` is not a function"
  else Left $ "Unknown function `" ++ fname ++ "`"
--
-- Math operations
chkExp s (EAdd e1 e2) = chkBinExp s e1 e2 $ mathResolver "+" -- add
chkExp s (ESub e1 e2) = chkBinExp s e1 e2 $ mathResolver "-" -- subtract
chkExp s (EMul e1 e2) = chkBinExp s e1 e2 $ mathResolver "*" -- multiply
chkExp s (EDiv e1 e2) = chkBinExp s e1 e2 $ mathResolver "/" -- division
chkExp s (ENeg e)     = chkUnMathExp "-" s e                 -- negation
--
-- comparison operations
chkExp s (EEq e1 e2)  = chkBinExp s e1 e2 $ cmpResolver "="  -- equal
chkExp s (ENeq e1 e2) = chkBinExp s e1 e2 $ cmpResolver "<>" -- not equal
chkExp s (EGt e1 e2)  = chkBinExp s e1 e2 $ cmpOrdResolver ">"  -- greater than
chkExp s (ELt e1 e2)  = chkBinExp s e1 e2 $ cmpOrdResolver "<"  -- lesser than
chkExp s (EGe e1 e2)  = chkBinExp s e1 e2 $ cmpOrdResolver ">=" -- greater/equal
chkExp s (ELe e1 e2)  = chkBinExp s e1 e2 $ cmpOrdResolver "<=" -- lesser/equal
--
-- logical operations
chkExp s (EAnd e1 e2) = chkBinExp s e1 e2 $ logResolver "And" -- and
chkExp s (EOr e1 e2)  = chkBinExp s e1 e2 $ logResolver "Or"  -- or
chkExp s (ENot e)     = chkUnLogExp "Not" s e                 -- not


{- INSTRUCTIONS CHECK -}

returnResolver :: DType -> DType -> Either String DType
-- "Return" type resolver. Second argument is an expected type.
returnResolver _ TNone = Left $ "Can't return outside a function."
returnResolver t1 t2
  | t1 == t2  = Right t1
  | otherwise = Left $ "Expected `" ++ show(t2) ++ "`, not `" ++ show(t1) ++ "`"

chkInstr :: TState -> Instr -> Either String Bool
-- Semantic check function for instructions.
--
-- Declaration-instruction block
chkInstr s (IDecBlk (DBlk d) i) = do
  s' <- chkDeclBlk s d
  _  <- chkInstr s' i
  return True
--
-- Instruction block
chkInstr s (IBlk i) =
  let
    chk :: TState -> [Instr] -> Either String Bool
    chk s []     = Right True
    chk s (i:xs) = do
      _ <- chkInstr s i
      chk s xs
  in chk s i
--
-- Assignment
chkInstr s (IAsg v e) = do
  vt <- chkExp s v
  et <- chkExp s e
  if vt == et then return True
              else Left $ "Can't assign `" ++ (show et) ++ "` to `" ++ show(vt) ++ "`"
--
-- Naked expression
chkInstr s (IExp e) = do
  vt <- chkExp s e
  return True
--
-- Return
chkInstr s (IRet e) = do
  v <- chkExp s e
  rt <- returnResolver v (tRType s)
  return True
--
-- If
chkInstr s (IIf e i) = do
  t <- chkExp s e
  if t == TBool then do
    _ <- chkInstr s i
    return True
  else Left $ "`If " ++ show(t) ++ "` is not a valid instruction"
--
-- If-else
chkInstr s (IIfEl e i1 i2) = do
  _ <- chkInstr s $ IIf e i1
  _ <- chkInstr s $ IIf e i2
  return True
--
-- While
chkInstr s (IWh e i) = do
  t <- chkExp s e
  if t == TBool then do
    _ <- chkInstr (setWhile s) i
    return True
  else Left $ "`While " ++ show(t) ++ "` is not a valid instruction"
--
-- Break
chkInstr s (IBrk) =
  if tWhile s then Right True
  else Left $ "Can't break outside a while loop"
--
-- Continue
chkInstr s (ICont) =
  if tWhile s then Right True
  else Left $ "Can't continue outside a while loop"
--
-- Print
chkInstr s (IPrint e) = do
  t <- chkExp s e
  return True
