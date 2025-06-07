open Interpreter

let is_zero = UnaryOp (fun x -> if x = 0 then 1 else 0)

let is_one = UnaryOp (fun x -> if x = 1 then 1 else 0)

let add = BinaryOp ( + )

let subtract = BinaryOp ( - )

let fibonacci_function =
  Lambda
    (Lambda
       (IfThenElse
          ( Apply (is_zero, tvar 0),
            Const 0,
            IfThenElse
              ( Apply (is_one, tvar 0),
                Const 1,
                Apply
                  ( Apply
                      ( add,
                        Apply (tvar 1, Apply (Apply (subtract, tvar 0), Const 1))
                      ),
                    Apply (tvar 1, Apply (Apply (subtract, tvar 0), Const 2)) )
              ) )))

let benchmark_term = Apply (Fix fibonacci_function, Const 30)

let () =
  match denote [] benchmark_term with
  | VConst n -> Printf.printf "%d\n" n
  | _ -> failwith "Expected a constant result!"
