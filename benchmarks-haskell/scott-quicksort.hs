import Interpreter

scottNil :: Term
scottNil = Lambda (Lambda (TVar 1))

scottCons :: Term
scottCons =
  Lambda (Lambda (Lambda (Lambda (Apply (Apply (TVar 0) (TVar 3)) (TVar 2)))))

lessThan :: Term
lessThan = BinaryOp (\x y -> if x < y then 1 else 0)

greaterEqual :: Term
greaterEqual = BinaryOp (\x y -> if x >= y then 1 else 0)

scottFilter :: Term
scottFilter =
  Fix
    ( Lambda
        ( Lambda
            ( Lambda
                ( Apply
                    (Apply (TVar 0) scottNil)
                    ( Lambda
                        ( Lambda
                            ( IfThenElse
                                (Apply (TVar 3) (TVar 1))
                                ( Apply
                                    (Apply scottCons (TVar 1))
                                    (Apply (Apply (TVar 4) (TVar 3)) (TVar 0))
                                )
                                (Apply (Apply (TVar 4) (TVar 3)) (TVar 0))
                            )
                        )
                    )
                )
            )
        )
    )

scottAppend :: Term
scottAppend =
  Fix
    ( Lambda
        ( Lambda
            ( Lambda
                ( Apply
                    (Apply (TVar 1) (TVar 0))
                    ( Lambda
                        ( Lambda
                            ( Apply
                                (Apply scottCons (TVar 1))
                                (Apply (Apply (TVar 4) (TVar 0)) (TVar 2))
                            )
                        )
                    )
                )
            )
        )
    )

scottQuicksort :: Term
scottQuicksort =
  Fix
    ( Lambda
        ( Lambda
            ( Apply
                (Apply (TVar 0) scottNil)
                ( Lambda
                    ( Lambda
                        ( Apply
                            ( Apply
                                scottAppend
                                ( Apply
                                    (TVar 3)
                                    ( Apply
                                        ( Apply
                                            scottFilter
                                            (Lambda (Apply (Apply lessThan (TVar 0)) (TVar 2)))
                                        )
                                        (TVar 0)
                                    )
                                )
                            )
                            ( Apply
                                (Apply scottCons (TVar 1))
                                ( Apply
                                    (TVar 3)
                                    ( Apply
                                        ( Apply
                                            scottFilter
                                            (Lambda (Apply (Apply greaterEqual (TVar 0)) (TVar 2)))
                                        )
                                        (TVar 0)
                                    )
                                )
                            )
                        )
                    )
                )
            )
        )
    )

sumInts :: Term
sumInts = BinaryOp (+)

scottSumList :: Term
scottSumList =
  Fix
    ( Lambda
        ( Lambda
            ( Apply
                (Apply (TVar 0) (Const 0))
                (Lambda (Lambda (Apply (Apply sumInts (TVar 1)) (Apply (TVar 3) (TVar 0)))))
            )
        )
    )

generateList :: Int -> Term
generateList n = go n scottNil
  where
    go 0 acc = acc
    go i acc = go (i - 1) (Apply (Apply scottCons (Const (i - 1))) acc)

benchmarkTerm :: Term
benchmarkTerm =
  Apply scottSumList (Apply scottQuicksort (generateList 3000))

main :: IO ()
main = case denote [] benchmarkTerm of
  VConst n -> print n
  _ -> error "Expected a constant result!"