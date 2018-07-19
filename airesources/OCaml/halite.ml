(** The type of energies in the game. *)
type energy = int

(** The type of the dimensions and indices of the map. *)
type dimension = int

(** The type of directions. *)
type direction = North | South | East | West

(** The type of locations. *)
type location = dimension * dimension

(** The dropoffs on the map. *)
module rec Dropoff : sig
  (** The type of dropoff IDs. *)
  type id = Dropoff of int
  type t = {
    id : id;            (** The ID of the dropoff. *)
    location : location (** The location of the dropoff. *)
  }
end = Dropoff

(** An entity on the map, owned by a player. *)
module rec Entity : sig
  (** The type of entity IDs. *)
  type id = Entity of int
  (** The type of entities. *)
  type t = {
    id : id;                     (** The ID of the entity. *)
    mutable location : location; (** The location of the entity. *)
    mutable energy : energy      (** The energy of the entity. *)
  }
end = Entity

(** A player in the game. *)
module rec Player : sig
  (** The type of player IDs. *)
  type id = Player of int
  (** The type of players. *)
  type t = {
    id : id;
    factory : location;
    mutable dropoffs : Dropoff.t list;
    mutable energy : energy;
    mutable entities : Entity.id list
  }
end = Player

(** A cell on the game map. *)
module rec Cell : sig
  type t = {
    mutable energy : energy;
    mutable entity : Entity.id option;
    mutable owner : Player.id option
  }
end = Cell

(** The game map. *)
module Map : sig
  (** The abstract type of the map. *)
  type t
  (** Create a map. *)
  val make : dimension -> dimension -> t
  (** Get the width of the map. *)
  val width : t -> dimension
  (** Get the height of the map. *)
  val height : t -> dimension
  (** Get the cell type at a location. *)
  val get_cell : t -> location -> Cell.t
end = struct
  type t = {
    width : dimension;
    height : dimension;
    cells : Cell.t array array;
  }
  let empty_cell : Cell.t = {energy=0; entity=None; owner=None}
  let make width height = {
    width;
    height;
    cells=Array.make height (Array.make width empty_cell)
  }
  let width {width} = width
  let height {height} = height
  let get_cell {cells} (x, y) = cells.(y).(x)
end

(** The game state. *)
module State : sig
  type t = {
    mutable turn_number : int;                  (** The current turn number *)
    player_id : Player.id;                      (** The player's ID. *)
    players : (Player.id, Player.t) Hashtbl.t;  (** The players in the game. *)
    entities : (Entity.id, Entity.t) Hashtbl.t; (** The entities in the game. *)
    map : Map.t                                 (** The game map. *)
  }
  val curr_player : t -> Player.t               (** Get the current player. *)
  val get_player : t -> Player.id -> Player.t   (** Get a player by ID. *)
  val get_entity : t -> Entity.id -> Entity.t   (** Get an entity by ID. *)
end = struct
  type t = {
    mutable turn_number : int;
    player_id : Player.id;
    players : (Player.id, Player.t) Hashtbl.t;
    entities : (Entity.id, Entity.t) Hashtbl.t;
    map : Map.t
  }
  let curr_player {players; player_id} = Hashtbl.find players player_id
  let get_player {players} id = Hashtbl.find players id
  let get_entity {entities} id = Hashtbl.find entities id
end

(** A command issued by a player. *)
module rec Command : sig
  (** The type of a command. *)
  type t =
    | Move of Entity.id * direction
    | Spawn of energy
    | Dump of Entity.id * energy
    | Construct of Entity.id
end = Command

(* Halite game constants. *)
module Constants = struct
  let entity_cost = 1000
end
