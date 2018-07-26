open Halite
let name () = "MyOCamlBot-" ^ string_of_int (match Player.id (Player.current ()) with Player id -> id)
let turn turn_number =
  let directions = [| North; South; East; West |] in
  let random_direction () = Array.get directions (Random.int (Array.length directions)) in
  let player = Player.current () in
  List.fold_left (fun cs e ->
      if Entity.location e = Player.factory player &&
         Entity.energy e > Constants.max_capacity / 4 then Dump (e, Entity.energy e)::cs
      else if Cell.energy (Entity.cell e) > Constants.max_capacity / 10 then cs
      else Move (e, random_direction ())::cs
    ) (if turn_number <= 200 && Player.energy player >= Constants.entity_cost then [Spawn 0] else []) (Player.entities (Player.current ()))
