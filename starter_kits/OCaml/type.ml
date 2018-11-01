(* OCaml Starter for Halite III on Halite.io
   This code is public domain. There is no warranty.
 *)

type direction = 
  | North 
  | East 
  | South 
  | West 
  | Still 
;;

type command = 
  | Generate 
  | Construct of int
  | Move of (int * direction)
;;

type entity_type = | Ship | Shipyard | Dropoff;;

(* the halite field is only used for ships, id field is not for shipyards *)
type entity = 
 {
  entity_type : entity_type;
  id : int;
  owner : int;
  mutable position : int * int; (* row, col *)
  mutable halite : int;
 }
;;

(* This is the data structure I originally used for entities. It seemed neat
 * at first and it doesn't waste as much memory for Shipyards or Dropoffs, 
 * but it requires pattern matching every time you want to access the 
 * entity_data. Wasting a bit of memory is preferable in this case - there
 * are very few shipyards or dropoffs, and lots of places in the code where
 * you want to access the entity data.
type entity_data = 
 {
  mutable id : int;
  mutable owner : int;
  mutable position : int * int; (* row, col *)
 }
;;
type entity =
  | Dropoff of entity_data
  | Shipyard of entity_data
  | Ship of (entity_data * int) (* the int is the halite_amount *)
;;
*)

type constant_set = {
  mutable capture_enabled : bool;
  mutable capture_radius : int;
  mutable default_map_height : int;
  mutable default_map_width : int;
  mutable dropoff_cost : int;
  mutable dropoff_penalty_ratio : int;
  mutable extract_ratio : int;
  mutable factor_exp_1 : float;
  mutable factor_exp_2 : float;
  mutable initial_energy : int;
  mutable inspiration_enabled : bool;
  mutable inspiration_radius : int;
  mutable inspiration_ship_count : int;
  mutable inspired_bonus_multiplier : float;
  mutable inspired_extract_ratio : int;
  mutable inspired_move_cost_ratio : int;
  mutable max_cell_production : int;
  mutable max_energy : int;
  mutable max_players : int;
  mutable max_turns : int;
  mutable max_turn_threshold : int;
  mutable min_cell_production : int;
  mutable min_turns : int;
  mutable min_turn_threshold : int;
  mutable move_cost_ratio : int;
  mutable new_entity_energy_cost : int;
  mutable persistence : float;
  mutable ships_above_for_capture : int;
  mutable strict_errors : bool;
  mutable game_seed : int;
};;

type mybot_state = { 
  (* These fields are not recommended; they should be replaced with 
   * whatever you would like to store in your bot's memory between turns. 
   *)
  mutable magicNumber : int;
  mutable magicList : int list;
  mutable previous_state : game_state option;
  mutable previous_commands : command list option;
}

and game_state = {
  mutable tokens : string list;
  mutable num_players : int;
  mutable my_id : int;
  mutable width : int;
  mutable height: int;
  mutable map : int array array;
  mutable ships : entity list;
  mutable shipyards : entity list;
  mutable dropoffs : entity list;
  mutable energy : int array;
  mutable turn : int;
  mutable last_update : float;
  mutable persist : mybot_state;
  const : constant_set;
};;


