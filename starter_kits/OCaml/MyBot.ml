(* OCaml Starter for Halite III on Halite.io
   This code is public domain. There is no warranty.
 *)

open Type;;

let mybot_fun state =

  let ship_commands = List.map (fun ship ->
    let (row, col) = ship.position in
    if (state.map.(row).(col) < state.const.max_energy / 10)
    || (ship.halite >= state.const.max_energy) then
      let dir = Game.random_nonstill_dir() in
        Move (ship.id, dir)
    else
      Move (ship.id, Still)
  ) (Game.my_ships state) in

  let other_commands =
    if (state.turn <= 200)
    && (Game.my_halite state >= state.const.new_entity_energy_cost)
    && not (Game.shipyard_occupied state state.my_id) then
      [Generate]
    else (
      []
    )
  in
    ship_commands @ other_commands
;;

let run bot =
  try (
    (* It is necessary log to a file for any medium-large amount of data *)
    Debug.setup_logging 0 "MyOcamlBot";
    let state = Game.new_state() in
    Random.init(max (state.const.game_seed - 7813) (state.const.game_seed + 8973));
    (* I was getting poor quality random results until I started discarding
     * the first few. 
     *)
    for i = 0 to 1000 do
      ignore(Random.int(i+10));
    done;
    Networking.process_initial_tokens state;
    Networking.send_string "ocamlStarterBot";
    Networking.done_sending();
    while true do
      Networking.parse_input state;
      let commands = bot state in
        Debug.debug (Printf.sprintf "Seconds elapsed = %f\n" (Game.time_seconds_elapsed_this_turn state));
        Networking.send_commands commands
    done
  )
  with e ->
    Debug.error (Printexc.to_string e);
    Debug.close_files();
;;

run mybot_fun;;

