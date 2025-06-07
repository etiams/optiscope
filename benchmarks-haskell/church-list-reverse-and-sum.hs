import Interpreter

churchNil :: Term
churchNil = Lambda (Lambda (TVar 0))

churchCons :: Term
churchCons =
  Lambda
    ( Lambda
        ( Lambda
            (Lambda (Apply (Apply (TVar 1) (TVar 3)) (Apply (Apply (TVar 2) (TVar 1)) (TVar 0))))
        )
    )

add :: Term
add = BinaryOp (+)

churchSumList :: Term
churchSumList =
  Lambda
    ( Apply (Apply (TVar 0) add) (Const 0)
    )

churchReverse :: Term
churchReverse =
  Lambda
    ( Apply
        ( Apply
            (TVar 0)
            ( Lambda
                ( Lambda
                    ( Lambda
                        ( Lambda
                            ( Apply
                                (Apply (TVar 2) (TVar 1))
                                (Apply (Apply (TVar 1) (TVar 3)) (TVar 0))
                            )
                        )
                    )
                )
            )
        )
        churchNil
    )

generateList :: Int -> Term
generateList n = go n churchNil
  where
    go 0 acc = acc
    go i acc = go (i - 1) (Apply (Apply churchCons (Const (i - 1))) acc)

benchmarkTerm :: Term
benchmarkTerm = Apply churchSumList (Apply churchReverse (generateList 1000000))

main :: IO ()
main = case denote [] benchmarkTerm of
  VConst n -> print n
  _ -> error "Expected a constant result!"
