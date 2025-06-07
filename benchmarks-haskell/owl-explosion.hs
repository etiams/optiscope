import Control.Exception (assert)
import Interpreter

owlCombinator :: Term
owlCombinator = Lambda (Lambda (Apply (TVar 0) (Apply (TVar 1) (TVar 0))))

owlExplosion :: Int -> Term
owlExplosion depth =
  assert (depth > 0) $ go 1 owlCombinator
  where
    go i acc
      | i >= depth = acc
      | otherwise = go (i + 1) (Apply acc owlCombinator)

benchmarkTerm :: Term
benchmarkTerm = Apply (owlExplosion 1000000) owlCombinator

main :: IO ()
main = print $ case denote [] benchmarkTerm of
  VClosure {} -> "VClosure"
  VUnaryOp {} -> "VUnaryOp"
  VBinaryOp {} -> "VBinaryOp"
  VBinaryOpAux {} -> "VBinaryOpAux"
  VConst n -> show n
