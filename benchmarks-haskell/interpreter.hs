{-# LANGUAGE LambdaCase #-}
{-# LANGUAGE PatternSynonyms #-}

module Interpreter where

newtype DeBruijnIndex = Index Int

data Term
  = Lambda Term
  | Apply Term Term
  | Var DeBruijnIndex
  | Fix Term
  | UnaryOp (Int -> Int)
  | BinaryOp (Int -> Int -> Int)
  | IfThenElse Term Term Term
  | Const Int

pattern TVar idx = Var (Index idx)

data Value
  = VClosure [Value] Term
  | VUnaryOp (Int -> Int)
  | VBinaryOp (Int -> Int -> Int)
  | VBinaryOpAux (Int -> Int -> Int) Int
  | VConst Int

denote :: [Value] -> Term -> Value
denote rho = \case
  Lambda m -> VClosure rho m
  Apply m n ->
    let (mVal, nVal) = (denote rho m, denote rho n)
     in case mVal of
          VClosure rho' body -> denote (nVal : rho') body
          VUnaryOp op ->
            case nVal of VConst x -> VConst (op x); _ -> error "Unacceptable!"
          VBinaryOp op ->
            case nVal of VConst x -> VBinaryOpAux op x; _ -> error "Unacceptable!"
          VBinaryOpAux op x ->
            case nVal of VConst y -> VConst (op x y); _ -> error "Unacceptable!"
          VConst _ -> error "Unacceptable!"
  Var (Index idx) -> rho !! idx
  Fix (Lambda body) ->
    let fixpoint = denote (fixpoint : rho) body
     in fixpoint
  Fix _ -> error "Unacceptable!"
  UnaryOp op -> VUnaryOp op
  BinaryOp op -> VBinaryOp op
  IfThenElse condition ifThen ifElse ->
    case denote rho condition of
      VConst n | n /= 0 -> denote rho ifThen
      VConst _ -> denote rho ifElse
      _ -> error "Unacceptable!"
  Const n -> VConst n
