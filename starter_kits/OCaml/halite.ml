include Units

(** The dropoffs on the map. *)
module rec Dropoff : sig
  (** The type of dropoff IDs. *)
  type id = Dropoff of int
  (** Exception raised when an ID is invalid. *)
  exception Invalid_id
  (** The abstract type of dropoffs. *)
  type t
  (** Get the ID of a dropoff. *)
  val id : t -> id
  (** Get a dropoff from ID. *)
  val of_id : id -> t
  (** Get all the dropoffs. *)
  val all : unit -> t list
  (** Get the location of a dropoff. *)
  val location : t -> location
  (** Get the owner of a dropoff. *)
  val owner : t -> Player.t
  (** Get the cell of a dropoff. *)
  val cell : t -> Cell.t
  (** Get the energy of a dropoff. *)
  val energy : t -> energy
end = HaliteImpl.Dropoff

(** An entity on the map, owned by a player. *)
and Entity : sig
  (** The type of entity IDs. *)
  type id = Entity of int
  (** Exception raised when an ID is invalid. *)
  exception Invalid_id
  (** The abstract type of entities. *)
  type t
  (** Get the ID of an entity. *)
  val id : t -> id
  (** Get an entity from ID. *)
  val of_id : id -> t
  (** Get all the entities. *)
  val all : unit -> t list
  (** Get the location of an entity. *)
  val location : t -> location
  (** Get the energy of an entity. *)
  val energy : t -> energy
  (** Get the owner of an entity. *)
  val owner : t -> Player.t
  (** Get the cell of an entity. *)
  val cell : t -> Cell.t
end = HaliteImpl.Entity

(** A player in the game. *)
and Player : sig
  (** The type of player IDs. *)
  type id = Player of int
  (** Exception raised when an ID is invalid. *)
  exception Invalid_id
  (** The type of players. *)
  type t
  (** Get the ID of a player. *)
  val id : t -> id
  (** Get a player from ID. *)
  val of_id : id -> t
  (** Get the current player. *)
  val current : unit -> t
  (** Get all the players. *)
  val all : unit -> t list
  (** Get the factory location of a player. *)
  val factory : t -> location
  (** Get the factory cell of a player. *)
  val factory_cell : t -> Cell.t
  (** Get the energy of a player. *)
  val energy : t -> energy
  (** Get the dropoffs of a player. *)
  val dropoffs : t -> Dropoff.t list
  (** Get the entities of a player. *)
  val entities : t -> Entity.t list
end = HaliteImpl.Player

(** A cell on the game map. *)
and Cell : sig
  (** The abstract type of a cell. *)
  type t
  (** Get the location of a cell. *)
  val location : t -> location
  (** Get the energy of a cell. *)
  val energy : t -> energy
  (** Get the entity on a cell. *)
  val entity : t -> Entity.t option
  (** Get the owner of a cell. *)
  val owner : t -> Player.t option
  (** Get the dropoff on a cell. *)
  val dropoff : t -> Dropoff.t option
end = HaliteImpl.Cell

(** The game board. *)
and Board : sig
  (** Get the width of the board. *)
  val width : unit -> dimension
  (** Get the height of the board. *)
  val height : unit -> dimension
  (** Get the cell at a location. *)
  val at : location -> Cell.t
end = HaliteImpl.Board

(** The type of a command. *)
type command =
  (** Move an entity in a direction. *)
  | Move of Entity.t * direction
  (** Spawn an entity with some energy. *)
  | Spawn of energy
  (** Dump some energy from an entity. *)
  | Dump of Entity.t * energy
  (** Construct a dropoff with an entity. *)
  | Construct of Entity.t

(* Halite game constants. *)
module Constants : sig
  (** The cost of an entity. *)
  val entity_cost : int
  (** The cost of a dropoff. *)
  val dropoff_cost : int
  (** The denominator of the move cost ratio. *)
  val move_cost_ratio : int
  (** The denominator of the per-turn mining ratio. *)
  val mine_ratio : int
  (** The maximum amount of energy per entity. *)
  val max_capacity : int
end = HaliteImpl.Constants
