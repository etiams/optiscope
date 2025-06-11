open Interpreter

let church_zero = Lambda (Lambda (tvar 0))

let church_one = Lambda (Lambda (Apply (tvar 1, tvar 0)))

let church_five =
  Lambda
    (Lambda
       (Apply
          ( tvar 1,
            Apply
              (tvar 1, Apply (tvar 1, Apply (tvar 1, Apply (tvar 1, tvar 0))))
          )))

let church_add =
  Lambda
    (Lambda
       (Lambda
          (Lambda
             (Apply
                (Apply (tvar 3, tvar 1), Apply (Apply (tvar 2, tvar 1), tvar 0))))))

let church_predecessor =
  Lambda
    (Lambda
       (Lambda
          (Apply
             ( Apply
                 ( Apply
                     ( tvar 2,
                       Lambda (Lambda (Apply (tvar 0, Apply (tvar 1, tvar 3))))
                     ),
                   Lambda (tvar 1) ),
               Lambda (tvar 0) ))))

let church_predecessor2x =
  Lambda (Apply (church_predecessor, Apply (church_predecessor, tvar 0)))

let church_true = Lambda (Lambda (tvar 1))

let church_false = Lambda (Lambda (tvar 0))

let church_is_zero =
  Lambda (Apply (Apply (tvar 0, Lambda church_false), church_true))

let church_is_one =
  Lambda (Apply (church_is_zero, Apply (church_predecessor, tvar 0)))

let church_if_then_else =
  Lambda (Lambda (Lambda (Apply (Apply (tvar 2, tvar 1), tvar 0))))

let y_fibonacci_function =
  Lambda
    (Lambda
       (Apply
          ( Apply
              ( Apply (church_if_then_else, Apply (church_is_zero, tvar 0)),
                church_zero ),
            Apply
              ( Apply
                  ( Apply (church_if_then_else, Apply (church_is_one, tvar 0)),
                    church_one ),
                Apply
                  ( Apply
                      ( church_add,
                        Apply (tvar 1, Apply (church_predecessor, tvar 0)) ),
                    Apply (tvar 1, Apply (church_predecessor2x, tvar 0)) ) ) )))

let church_ten = Apply (Apply (church_add, church_five), church_five)

let church_twenty = Apply (Apply (church_add, church_ten), church_ten)

let i_combinator = Lambda (tvar 0)

let y_combinator =
  Lambda
    (Apply
       ( Lambda (Apply (tvar 1, Apply (tvar 0, tvar 0))),
         Lambda (Apply (tvar 1, Apply (tvar 0, tvar 0))) ))

let benchmark_term =
  Apply
    ( Apply
        ( Apply (Apply (y_combinator, y_fibonacci_function), church_twenty),
          i_combinator ),
      i_combinator )

let () =
  match denote [] benchmark_term with
  | VClosure (_, _) -> print_endline "\\x -> x"
  | _ -> failwith "Expected a lambda term!"
