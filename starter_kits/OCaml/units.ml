(** The type of energies in the game. *)
type energy = int

(** The type of the dimensions and indices of the map. *)
type dimension = int

(** The type of directions. *)
type direction = North | South | East | West

(** The type of locations. *)
type location = {x : dimension; y : dimension}
