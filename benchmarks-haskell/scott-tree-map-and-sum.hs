{-# LANGUAGE LambdaCase #-}

import Interpreter

scottLeaf :: Term
scottLeaf = Lambda (Lambda (Lambda (Apply (TVar 1) (TVar 2))))

scottNode :: Term
scottNode =
  Lambda (Lambda (Lambda (Lambda (Apply (Apply (TVar 0) (TVar 3)) (TVar 2)))))

add :: Term
add = BinaryOp (+)

multiply :: Term
multiply = BinaryOp (*)

scottTreeSum :: Term
scottTreeSum =
  Fix
    ( Lambda
        ( Lambda
            ( Apply
                (Apply (TVar 0) (Lambda (TVar 0)))
                ( Lambda
                    ( Lambda
                        (Apply (Apply add (Apply (TVar 3) (TVar 1))) (Apply (TVar 3) (TVar 0)))
                    )
                )
            )
        )
    )

scottTreeMap :: Term
scottTreeMap =
  Fix
    ( Lambda
        ( Lambda
            ( Lambda
                ( Apply
                    (Apply (TVar 0) (Lambda (Apply scottLeaf (Apply (TVar 2) (TVar 0)))))
                    ( Lambda
                        ( Lambda
                            ( Apply
                                (Apply scottNode (Apply (Apply (TVar 4) (TVar 3)) (TVar 1)))
                                (Apply (Apply (TVar 4) (TVar 3)) (TVar 0))
                            )
                        )
                    )
                )
            )
        )
    )

doubleFunction :: Term
doubleFunction = Lambda (Apply (Apply multiply (TVar 0)) (Const 2))

generateTree :: Int -> Term
generateTree = \case
  1 -> Apply scottLeaf (Const 1)
  n ->
    Apply
      (Apply scottNode (generateTree (n `div` 2)))
      (generateTree (n `div` 2))

benchmarkTerm :: Term
benchmarkTerm =
  Apply
    scottTreeSum
    (Apply (Apply scottTreeMap doubleFunction) (generateTree (65536 * 2 * 2)))

main :: IO ()
main = case denote [] benchmarkTerm of
  VConst n -> print n
  _ -> error "Expected a constant result!"
