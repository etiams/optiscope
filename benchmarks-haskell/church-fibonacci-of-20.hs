import Interpreter

churchZero :: Term
churchZero = Lambda (Lambda (TVar 0))

churchOne :: Term
churchOne = Lambda (Lambda (Apply (TVar 1) (TVar 0)))

churchFive :: Term
churchFive =
  Lambda
    ( Lambda
        ( Apply
            (TVar 1)
            (Apply (TVar 1) (Apply (TVar 1) (Apply (TVar 1) (Apply (TVar 1) (TVar 0)))))
        )
    )

churchAdd :: Term
churchAdd =
  Lambda
    ( Lambda
        ( Lambda
            ( Lambda
                ( Apply (Apply (TVar 3) (TVar 1)) (Apply (Apply (TVar 2) (TVar 1)) (TVar 0))
                )
            )
        )
    )

churchPredecessor :: Term
churchPredecessor =
  Lambda
    ( Lambda
        ( Lambda
            ( Apply
                ( Apply
                    (Apply (TVar 2) (Lambda (Lambda (Apply (TVar 0) (Apply (TVar 1) (TVar 3))))))
                    (Lambda (TVar 1))
                )
                (Lambda (TVar 0))
            )
        )
    )

churchPredecessor2x :: Term
churchPredecessor2x = Lambda (Apply churchPredecessor (Apply churchPredecessor (TVar 0)))

churchTrue :: Term
churchTrue = Lambda (Lambda (TVar 1))

churchFalse :: Term
churchFalse = Lambda (Lambda (TVar 0))

churchIsZero :: Term
churchIsZero = Lambda (Apply (Apply (TVar 0) (Lambda churchFalse)) churchTrue)

churchIsOne :: Term
churchIsOne = Lambda (Apply churchIsZero (Apply churchPredecessor (TVar 0)))

churchIfThenElse :: Term
churchIfThenElse = Lambda (Lambda (Lambda (Apply (Apply (TVar 2) (TVar 1)) (TVar 0))))

yFibonacciFunction :: Term
yFibonacciFunction =
  Lambda
    ( Lambda
        ( Apply
            (Apply (Apply churchIfThenElse (Apply churchIsZero (TVar 0))) churchZero)
            ( Apply
                (Apply (Apply churchIfThenElse (Apply churchIsOne (TVar 0))) churchOne)
                ( Apply
                    (Apply churchAdd (Apply (TVar 1) (Apply churchPredecessor (TVar 0))))
                    (Apply (TVar 1) (Apply churchPredecessor2x (TVar 0)))
                )
            )
        )
    )

churchTen :: Term
churchTen = Apply (Apply churchAdd churchFive) churchFive

churchTwenty :: Term
churchTwenty = Apply (Apply churchAdd churchTen) churchTen

iCombinator :: Term
iCombinator = Lambda (TVar 0)

yCombinator :: Term
yCombinator =
  Lambda
    ( Apply
        (Lambda (Apply (TVar 1) (Apply (TVar 0) (TVar 0))))
        (Lambda (Apply (TVar 1) (Apply (TVar 0) (TVar 0))))
    )

benchmarkTerm :: Term
benchmarkTerm =
  Apply (Apply (Apply (Apply yCombinator yFibonacciFunction) churchTwenty) iCombinator) iCombinator

main :: IO ()
main = case denote [] benchmarkTerm of
  VClosure _ (Var (Index 0)) -> putStrLn "\\x -> x"
  _ -> error "Expected a lambda term!"
