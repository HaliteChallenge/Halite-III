open Halite
val name : State.t -> string
val init : State.t -> unit
val turn : State.t -> Command.t list
