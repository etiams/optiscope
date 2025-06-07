open Interpreter

let scott_leaf = Lambda (Lambda (Lambda (Apply (tvar 1, tvar 2))))

let scott_node =
  Lambda (Lambda (Lambda (Lambda (Apply (Apply (tvar 0, tvar 3), tvar 2)))))

let add = BinaryOp ( + )

let multiply = BinaryOp ( * )

let scott_tree_sum =
  Fix
    (Lambda
       (Lambda
          (Apply
             ( Apply (tvar 0, Lambda (tvar 0)),
               Lambda
                 (Lambda
                    (Apply
                       ( Apply (add, Apply (tvar 3, tvar 1)),
                         Apply (tvar 3, tvar 0) ))) ))))

let scott_tree_map =
  Fix
    (Lambda
       (Lambda
          (Lambda
             (Apply
                ( Apply
                    (tvar 0, Lambda (Apply (scott_leaf, Apply (tvar 2, tvar 0)))),
                  Lambda
                    (Lambda
                       (Apply
                          ( Apply
                              ( scott_node,
                                Apply (Apply (tvar 4, tvar 3), tvar 1) ),
                            Apply (Apply (tvar 4, tvar 3), tvar 0) ))) )))))

let double_function = Lambda (Apply (Apply (multiply, tvar 0), Const 2))

let rec generate_tree = function
  | 1 -> Apply (scott_leaf, Const 1)
  | n -> Apply (Apply (scott_node, generate_tree (n / 2)), generate_tree (n / 2))

let benchmark_term =
  Apply
    ( scott_tree_sum,
      Apply (Apply (scott_tree_map, double_function), generate_tree (65536 * 2 * 2)) )

let () =
  match denote [] benchmark_term with
  | VConst n -> Printf.printf "%d\n" n
  | _ -> failwith "Expected a constant result!"
