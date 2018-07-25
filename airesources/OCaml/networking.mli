open Halite

(** Get an initial game state. *)
val init_state : unit -> unit
(** Update the game state for a new turn, returning the turn number. *)
val update_state : unit -> int
(** Send commands to the engine. *)
val send_commands : command list -> unit
(** Send the player name to the engine. *)
val send_name : string -> unit
