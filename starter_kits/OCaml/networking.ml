(* OCaml Starter for Halite III on Halite.io
   This code is public domain. There is no warranty.
 *)

open Type;;

let read_tokens () =
  let input = String.trim (read_line ()) in
  String.split_on_char ' ' input
;;

let rec get_token state =
  match state.tokens with
  | [] -> 
    state.tokens <- read_tokens();
    get_token state
  | token :: tail ->
    state.tokens <- tail;
    token
;;

let get_int_token state =
  int_of_string (get_token state)
;;

let strip_first_char str =
  if str = "" then "" else
  String.sub str 1 ((String.length str) - 1)
;;

let strip_last_char str =
  if str = "" then "" else
  String.sub str 0 ((String.length str) - 1)
;;

let process_constant state left right =
  match strip_first_char (strip_last_char left) with
  | "CAPTURE_ENABLED" -> state.const.capture_enabled <- bool_of_string right
  | "CAPTURE_RADIUS" -> state.const.capture_radius <- int_of_string right
  | "DEFAULT_MAP_HEIGHT" -> state.const.default_map_height <- int_of_string right
  | "DEFAULT_MAP_WIDTH" -> state.const.default_map_width <- int_of_string right
  | "DROPOFF_COST" -> state.const.dropoff_cost <- int_of_string right
  | "DROPOFF_PENALTY_RATIO" -> state.const.dropoff_penalty_ratio <- int_of_string right
  | "EXTRACT_RATIO" -> state.const.extract_ratio <- int_of_string right
  | "FACTOR_EXP_1" -> state.const.factor_exp_1 <- float_of_string right
  | "FACTOR_EXP_2" -> state.const.factor_exp_2 <- float_of_string right
  | "INITIAL_ENERGY" -> state.const.initial_energy <- int_of_string right
  | "INSPIRATION_ENABLED" -> state.const.inspiration_enabled <- bool_of_string right
  | "INSPIRATION_RADIUS" -> state.const.inspiration_radius <- int_of_string right
  | "INSPIRATION_SHIP_COUNT" -> state.const.inspiration_ship_count <- int_of_string right
  | "INSPIRED_BONUS_MULTIPLIER" -> state.const.inspired_bonus_multiplier <- float_of_string right
  | "INSPIRED_EXTRACT_RATIO" -> state.const.inspired_extract_ratio <- int_of_string right
  | "INSPIRED_MOVE_COST_RATIO" -> state.const.inspired_move_cost_ratio <- int_of_string right
  | "MAX_CELL_PRODUCTION" -> state.const.max_cell_production <- int_of_string right
  | "MAX_ENERGY" -> state.const.max_energy <- int_of_string right
  | "MAX_PLAYERS" -> state.const.max_players <- int_of_string right
  | "MAX_TURNS" -> state.const.max_turns <- int_of_string right
  | "MAX_TURN_THRESHOLD" -> state.const.max_turn_threshold <- int_of_string right
  | "MIN_CELL_PRODUCTION" -> state.const.min_cell_production <- int_of_string right
  | "MIN_TURNS" -> state.const.min_turns <- int_of_string right
  | "MIN_TURN_THRESHOLD" -> state.const.min_turn_threshold <- int_of_string right
  | "MOVE_COST_RATIO" -> state.const.move_cost_ratio <- int_of_string right
  | "NEW_ENTITY_ENERGY_COST" -> state.const.new_entity_energy_cost <- int_of_string right
  | "PERSISTENCE" -> state.const.persistence <- float_of_string right
  | "SHIPS_ABOVE_FOR_CAPTURE" -> state.const.ships_above_for_capture <- int_of_string right
  | "STRICT_ERRORS" -> state.const.strict_errors <- bool_of_string right
  | "game_seed" -> state.const.game_seed <- int_of_string right
  | _ -> ()
;;

let rec process_constants state tokens =
  match tokens with
  | pair :: tail ->
    begin match String.split_on_char ':' pair with
    | left :: right :: [] ->
      process_constant state left right;
      process_constants state tail
    | _ -> () 
    end
  | [] -> ()
;;

let process_json state json =
  let preprocess = strip_first_char (strip_last_char json) in
  let tokens = String.split_on_char ',' preprocess in
    process_constants state tokens;
;;

let new_entity etype pid id row col halite =
 {
  entity_type = etype;
  id = id;
  owner = pid;
  position = (row, col);
  halite = halite;
 }
;;

let new_shipyard pid row col =
  (new_entity Shipyard pid pid row col 0)
;;

let new_ship pid id row col halite =
  (new_entity Ship pid id row col halite)
;;

let new_dropoff pid id row col =
  (new_entity Dropoff pid id row col 0)
;;

let rec process_player_data state num_players =
  if num_players < 1 then ()
  else (
    let player_id = get_int_token state in
    let player_col = get_int_token state in
    let player_row = get_int_token state in
    state.shipyards <- (new_shipyard player_id player_row player_col) :: state.shipyards;
    process_player_data state (num_players - 1)
  )
;;

let parse_map state =
  state.map <- Array.init state.height (fun i -> Array.init state.width (fun j -> 0));
  for row = 0 to state.height - 1 do
    for col = 0 to state.width - 1 do
      state.map.(row).(col) <- get_int_token state
    done
  done
;;

let process_players_and_map state =
  state.num_players <- get_int_token state;
  state.energy <- Array.init state.num_players (fun _ -> state.const.initial_energy);
  state.my_id <- get_int_token state;
  process_player_data state state.num_players;
  state.width <- get_int_token state;
  state.height <- get_int_token state;
  parse_map state
;;

let process_initial_tokens state = 
  let json = String.trim (read_line ()) in
    process_json state json;
    process_players_and_map state;
;;

let parse_input state =

  state.turn <- get_int_token state;
  Debug.debug (Printf.sprintf "==== Turn %03d ====\n" state.turn);

  state.last_update <- Unix.gettimeofday ();
  state.dropoffs <- [];
  state.ships <- [];

  for count = 0 to state.num_players - 1 do
    let player_id = get_int_token state in
    let num_ships = get_int_token state in
    let num_dropoffs = get_int_token state in
    state.energy.(player_id) <- get_int_token state;

    for _ = 0 to num_ships - 1 do
      let ship_id = get_int_token state in
      let col = get_int_token state in
      let row = get_int_token state in
      let halite = get_int_token state in
      state.ships <- new_ship player_id ship_id row col halite :: state.ships;
    done;

    for _ = 0 to num_dropoffs - 1 do
      let id = get_int_token state in
      let col = get_int_token state in
      let row = get_int_token state in
      state.dropoffs <- new_dropoff player_id id row col :: state.dropoffs;
    done
  done;

  let num_map_updates = get_int_token state in

  for _ = 0 to num_map_updates - 1 do
    let col = get_int_token state in
    let row = get_int_token state in
    let halite = get_int_token state in
    state.map.(row).(col) <- halite
  done

;;

let send_string s = 
  Printf.printf "%s" s
;;

let done_sending () =
  send_string "\n";
  flush stdout;
;;

let string_of_command = function
  | Generate -> "g"
  | Construct ship_id -> "c " ^ (string_of_int ship_id) ^ " "
  | Move (ship_id, dir) -> 
    "m " ^ (string_of_int ship_id) ^ " " ^ begin match dir with
      | North -> "n"
      | East -> "e"
      | South -> "s"
      | West -> "w"
      | Still -> "o"
      end ^ " "
;;

let send_commands commands =
  List.iter (fun command ->
    send_string (string_of_command command)
  ) commands;
  done_sending ()
;;

