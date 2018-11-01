(* OCaml Starter for Halite III on Halite.io
   This code is public domain. There is no warranty.
 *)

open Type;;

let time_seconds_elapsed_this_turn state =
  Unix.gettimeofday () -. state.last_update
;;

let wrap_position state (row, col) =
  let wr =
    if row < 0 then
      row + state.height
    else if row >= state.height then
      row - state.height
    else row
  in
  let wc =
    if col < 0 then
      col + state.width
    else if col >= state.width then
      col - state.width
    else col 
  in
  wr, wc
;;

let add_position (r1, c1) (r2, c2) = 
  (r1 + r2), (c1 + c2)
;;

let subtract_position (r1, c1) (r2, c2) =
  (r1 - r2), (c1 - c2)
;;

let dir_offset = function
| North -> (-1, 0)
| East -> (0, 1)
| South -> (1, 0)
| West -> (0, -1)
| Still -> (0, 0)
;;

let directional_offset_position state position direction =
  wrap_position state (add_position position (dir_offset direction))
;;

let abs_position (r, c) = (abs r, abs c);;

(* this logic was copied over from the Python starter, it has not been tested *)
let calculate_distance state source target =
  let source' = wrap_position state source in
  let target' = wrap_position state target in
  let row, col = abs_position (subtract_position source' target') in
  (min row (state.height - row)) + (min col (state.width - col))
;;

let ships_of_player state player =
  List.filter (fun entity ->
      entity.owner = player
  ) state.ships
;;

let my_ships state =
  ships_of_player state state.my_id
;;

let int_to_dir = function
  | 0 -> North
  | 1 -> East
  | 2 -> South
  | 3 -> West
  | _ -> Still
;;

let random_nonstill_dir () =
  int_to_dir (Random.int 4)
;;

let my_halite state =
  state.energy.(state.my_id)
;;

let get_shipyard state player =
  List.find (fun entity ->
      entity.owner = player
  ) state.shipyards
;;

let shipyard_occupied state player =
  try (
    let shipyard = get_shipyard state player in
      let result = (List.exists (fun ship ->
        ship.position = shipyard.position
      ) state.ships)
      in
        result
  )
  with e -> 
    Debug.debug (Printexc.to_string e);
    true
;;

let new_constant_set () = {
  capture_enabled = false;
  capture_radius = 0;
  default_map_height = 0;
  default_map_width = 0;
  dropoff_cost = 0;
  dropoff_penalty_ratio = 0;
  extract_ratio = 0;
  factor_exp_1 = 0.0;
  factor_exp_2 = 0.0;
  initial_energy = 0;
  inspiration_enabled = false;
  inspiration_radius = 0;
  inspiration_ship_count = 0;
  inspired_bonus_multiplier = 0.0;
  inspired_extract_ratio = 0;
  inspired_move_cost_ratio = 0;
  max_cell_production = 0;
  max_energy = 0;
  max_players = 0;
  max_turns = 0;
  max_turn_threshold = 0;
  min_cell_production = 0;
  min_turns = 0;
  min_turn_threshold = 0;
  move_cost_ratio = 0;
  new_entity_energy_cost = 0;
  persistence = 0.0;
  ships_above_for_capture = 0;
  strict_errors = false;
  game_seed = 0
};;

let new_state () = {
  tokens = [];
  num_players = 0;
  my_id = -1;
  width = 1;
  height = 1;
  map = [||];
  shipyards = [];
  ships = [];
  dropoffs = [];
  energy = [||];
  turn = -1;
  last_update = Unix.gettimeofday();
  persist = { 
    magicNumber = 42;
    magicList = [];
    previous_state = None;
    previous_commands = None;
  };
  const = new_constant_set();
};;

