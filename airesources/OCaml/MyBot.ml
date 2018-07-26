open Halite
open Networking

let name () = "MyOCamlBot-" ^ string_of_int (match Player.id (Player.current ()) with Player id -> id)
let turn turn_number =
  let directions = [| North; South; East; West |] in
  let random_direction () = Array.get directions (Random.int (Array.length directions)) in
  let player = Player.current () in
  let entities = Player.entities player in
  List.fold_left (fun cs e ->
      if Entity.location e = Player.factory player &&
         Entity.energy e > Constants.max_capacity / 4 then Dump (e, Entity.energy e)::cs
      else if Cell.energy (Entity.cell e) > Constants.max_capacity / 10 then cs
      else Move (e, random_direction ())::cs
    ) (if (List.length entities = 0 || (turn_number > 50 && Cell.entity (Player.factory_cell player) = None)) &&
          turn_number <= 200 && Player.energy player >= Constants.entity_cost
       then [Spawn 0] else []) entities

let () =
  let state = get_init () in
  send_init (name state);
  while true do
    let turn_number = get_frame state in
    end_turn (turn turn_number);
  done
