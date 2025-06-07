type de_bruijn_index = Index of int

type term =
  | Lambda of term
  | Apply of term * term
  | Var of de_bruijn_index
  | Fix of term
  | UnaryOp of (int -> int)
  | BinaryOp of (int -> int -> int)
  | IfThenElse of term * term * term
  | Const of int

let tvar idx = Var (Index idx)

type value =
  | VClosure of value list * term
  | VUnaryOp of (int -> int)
  | VBinaryOp of (int -> int -> int)
  | VBinaryOpAux of (int -> int -> int) * int
  | VConst of int
  | VThunk of (unit -> value)

let extract_int = function VConst n -> n | _ -> failwith "Unacceptable!"

let rec force = function VThunk f -> force (f ()) | value -> value

let rec denote rho = function
  | Lambda m -> VClosure (rho, m)
  | Apply (m, n) -> (
      let m_val = force (denote rho m) in
      let n_val = denote rho n in
      match m_val with
      | VClosure (rho', body) -> denote (n_val :: rho') body
      | VUnaryOp op -> VConst (op (extract_int (force n_val)))
      | VBinaryOp op -> VBinaryOpAux (op, extract_int (force n_val))
      | VBinaryOpAux (op, x) -> VConst (op x (extract_int (force n_val)))
      | VConst _ -> failwith "Unacceptable!"
      | VThunk _ -> failwith "Unacceptable!")
  | Var (Index idx) -> List.nth rho idx
  | Fix (Lambda body) ->
      let rec fixpoint_thunk () = denote (VThunk fixpoint_thunk :: rho) body in
      fixpoint_thunk ()
  | Fix _ -> failwith "Unacceptable!"
  | UnaryOp op -> VUnaryOp op
  | BinaryOp op -> VBinaryOp op
  | IfThenElse (condition, if_then, if_else) -> (
      match force (denote rho condition) with
      | VConst n when n <> 0 -> denote rho if_then
      | VConst _ -> denote rho if_else
      | _ -> failwith "Unacceptable!")
  | Const n -> VConst n
