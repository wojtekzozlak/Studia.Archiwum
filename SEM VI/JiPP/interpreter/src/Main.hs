module Main where

import Grammar.LexGrammar
import Grammar.ParGrammar
import Grammar.ErrM
import Grammar.AbsGrammar
import Data.Map

import RunTypes
import Interpreter
import StaticCheck

main = do
-- Main function
  interact parse
  putStrLn ""

parse s =
  let tree = pProg (myLexer s) -- program tree
      tstate = TState { tEnv = empty :: Env, tDefs = defDecls,
                        tRType = TNone, tWhile = False } -- initial typecheck state
      state = State { env = empty :: Env, store = empty :: Store,
                      defs = defDecls, flags = None } -- initial run state
  in case tree of 
    Ok (Prog e) -> show $ do
      _ <- chkInstr tstate e -- static type check
      s' <- iFun state e     -- actual evaluation
      return "Program evaluated successfuly"
    Bad e -> e
