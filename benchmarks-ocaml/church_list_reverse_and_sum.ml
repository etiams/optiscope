open Interpreter

let church_nil = Lambda (Lambda (tvar 0))

let church_cons =
  Lambda
    (Lambda
       (Lambda
          (Lambda
             (Apply
                (Apply (tvar 1, tvar 3), Apply (Apply (tvar 2, tvar 1), tvar 0))))))

let add = BinaryOp ( + )

let church_sum_list = Lambda (Apply (Apply (tvar 0, add), Const 0))

let church_reverse =
  Lambda
    (Apply
       ( Apply
           ( tvar 0,
             Lambda
               (Lambda
                  (Lambda
                     (Lambda
                        (Apply
                           ( Apply (tvar 2, tvar 1),
                             Apply (Apply (tvar 1, tvar 3), tvar 0) ))))) ),
         church_nil ))

let rec generate_list n =
  let rec go i acc =
    if i = 0 then acc
    else go (i - 1) (Apply (Apply (church_cons, Const (i - 1)), acc))
  in
  go n church_nil

let benchmark_term =
  Apply (church_sum_list, Apply (church_reverse, generate_list 1000000))

let () =
  match denote [] benchmark_term with
  | VConst n -> Printf.printf "%d\n" n
  | _ -> failwith "Expected a constant result!"
