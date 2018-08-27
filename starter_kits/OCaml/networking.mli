open Halite

(** Get an initial game state. *)
val get_init : unit -> unit
(** Update the game state for a new turn, returning the turn number. *)
val get_frame : unit -> int
(** Send commands to the engine. *)
val end_turn : command list -> unit
(** Send the player name to the engine. *)
val send_init : string -> unit
