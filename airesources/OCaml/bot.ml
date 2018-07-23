open Halite
open Networking

let name () : string =
  "MyBot " ^ string_of_int (match Player.id (Player.current ()) with Player i -> i)

let init () =
  (* Spawn one entity *)
  send_commands [Spawn 0]

let turn turn_number : command list =
  (* Get a random direction for each entity owned by the player *)
  let random_direction () =
    let directions = [|North; South; East; West|] in
    let n = Random.int (Array.length directions) in
    Array.get directions n in
  List.fold_left (fun cs entity -> Move (entity, random_direction ())::cs) [] (Player.entities (Player.current ()))
