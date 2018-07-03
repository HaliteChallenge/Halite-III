open Halite
(** Get an initial game state. *)
val init_state : unit -> State.t
(** Update the game state for a new turn. *)
val update_state : State.t -> unit
(** Send commands to the engine. *)
val send_commands : Command.t list -> unit
(** Send the player name to the engine. *)
val send_name : string -> unit
