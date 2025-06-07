open Interpreter

let owl_combinator = Lambda (Lambda (Apply (tvar 0, Apply (tvar 1, tvar 0))))

let rec owl_explosion depth =
  assert (depth > 0);
  let rec go i acc =
    if i >= depth then acc else go (i + 1) (Apply (acc, owl_combinator))
  in
  go 1 owl_combinator

let benchmark_term = Apply (owl_explosion 10000, owl_combinator)

let () = ignore (Sys.opaque_identity (denote [] benchmark_term))
