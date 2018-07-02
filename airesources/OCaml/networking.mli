open Halite
(** Get an initial game state. *)
val init_state : unit -> State.state
(** Update the game state for a new turn. *)
val update_state : State.state -> unit
(** Send commands to the engine. *)
val send_commands : Command.command list -> unit
