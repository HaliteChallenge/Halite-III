open Halite
open Networking

let name (state : State.t) : string =
  "MyBot " ^ string_of_int (match state.player_id with Player i -> i)

let init (state : State.t) =
  (* Spawn one entity *)
  send_commands [Command.Spawn ((State.curr_player state).energy - Constants.entity_cost)]

let turn (state : State.t) : Command.t list =
  (* Get a random direction for each entity owned by the player *)
  let random_direction () =
    let directions = [|North; South; East; West|] in
    let n = Random.int (Array.length directions) in
    Array.get directions n in
  List.fold_left (fun cs entity -> Command.Move (entity, random_direction ())::cs) [] (State.curr_player state).entities
