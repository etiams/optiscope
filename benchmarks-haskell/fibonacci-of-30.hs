import Interpreter

isZero :: Term
isZero = UnaryOp (\x -> if x == 0 then 1 else 0)

isOne :: Term
isOne = UnaryOp (\x -> if x == 1 then 1 else 0)

add :: Term
add = BinaryOp (+)

subtract' :: Term
subtract' = BinaryOp (-)

fibonacciFunction :: Term
fibonacciFunction =
  Lambda
    ( Lambda
        ( IfThenElse
            (Apply isZero (TVar 0))
            (Const 0)
            ( IfThenElse
                (Apply isOne (TVar 0))
                (Const 1)
                ( Apply
                    ( Apply add (Apply (TVar 1) (Apply (Apply subtract' (TVar 0)) (Const 1)))
                    )
                    (Apply (TVar 1) (Apply (Apply subtract' (TVar 0)) (Const 2)))
                )
            )
        )
    )

benchmarkTerm :: Term
benchmarkTerm = Apply (Fix fibonacciFunction) (Const 30)

main :: IO ()
main = case denote [] benchmarkTerm of
  VConst n -> print n
  _ -> error "Expected a constant result!"
