open Interpreter

let scott_nil = Lambda (Lambda (tvar 1))

let scott_cons =
  Lambda (Lambda (Lambda (Lambda (Apply (Apply (tvar 0, tvar 3), tvar 2)))))

let scott_singleton = Lambda (Apply (Apply (scott_cons, tvar 0), scott_nil))

let less_equal = BinaryOp (fun x y -> if x <= y then 1 else 0)

let scott_insert =
  Fix
    (Lambda
       (Lambda
          (Lambda
             (Apply
                ( Apply (tvar 0, Apply (scott_singleton, tvar 1)),
                  Lambda
                    (Lambda
                       (IfThenElse
                          ( Apply (Apply (less_equal, tvar 3), tvar 1),
                            Apply
                              ( Apply (scott_cons, tvar 3),
                                Apply (Apply (scott_cons, tvar 1), tvar 0) ),
                            Apply
                              ( Apply (scott_cons, tvar 1),
                                Apply (Apply (tvar 4, tvar 3), tvar 0) ) ))) )))))

let scott_insertion_sort =
  Fix
    (Lambda
       (Lambda
          (Apply
             ( Apply (tvar 0, scott_nil),
               Lambda
                 (Lambda
                    (Apply (Apply (scott_insert, tvar 1), Apply (tvar 3, tvar 0))))
             ))))

let sum_ints = BinaryOp ( + )

let scott_sum_list =
  Fix
    (Lambda
       (Lambda
          (Apply
             ( Apply (tvar 0, Const 0),
               Lambda
                 (Lambda
                    (Apply (Apply (sum_ints, tvar 1), Apply (tvar 3, tvar 0))))
             ))))

let rec generate_list n =
  let rec go i acc =
    if i = 0 then acc
    else go (i - 1) (Apply (Apply (scott_cons, Const (i - 1)), acc))
  in
  go n scott_nil

let benchmark_term =
  Apply (scott_sum_list, Apply (scott_insertion_sort, generate_list 1000000))

let () =
  match denote [] benchmark_term with
  | VConst n -> Printf.printf "%d\n" n
  | _ -> failwith "Expected a constant result!"
