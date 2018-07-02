(** Unit types used throughout Halite. *)
module rec Units : sig
  (** The type of energies in the game. *)
  type energy = int
  (** The type of the dimensions and indices of the map. *)
  type dimension = int
end = Units

(** The directions in which entities may move. *)
module rec Direction : sig
  (** The type of directions. *)
  type direction = North | South | East | West
end = Direction

(** The locations on the game map. *)
module rec Location : sig
  (** The type of locations. *)
  type location = Units.dimension * Units.dimension
end = Location

(** A cell on the game map. *)
module rec Cell : sig
  type cell =
    | Normal of Units.energy             (** A normal cell. *)
    | Obstacle of Units.energy           (** An obstacle cell. *)
    | Factory                            (** A factory cell. *)
    | EnergyFactor of Units.energy * int (** An energy factor cell. *)
end = Cell

(** An entity on the map, owned by a player. *)
module rec Entity : sig
  (** The type of entities. *)
  type entity = {
    owner : Player.id;                    (** The owner of the entity. *)
    mutable location : Location.location; (** The location of the entity. *)
    mutable energy : Units.energy         (** The energy of the entity. *)
  }
end = Entity

(** A player in the game. *)
and Player : sig
  (** The type of player IDs. *)
  type id = int
  (** The type of players. *)
  type player = {
    id : id;                                                        (** The ID of the player. *)
    mutable energy : Units.energy;                                  (** The stockpiled energy of the player. *)
    factory_location : Location.location;                           (** The location of the player factory. *)
    mutable entities : (Location.location, Entity.entity) Hashtbl.t (** The entities owned by the player. *)
  }
end = Player

(** The game map. *)
module Map : sig
  (** The abstract type of the map. *)
  type map
  (** Create a map. *)
  val make : Units.dimension -> Units.dimension -> map
  (** Get the width of the map. *)
  val width : map -> Units.dimension
  (** Get the height of the map. *)
  val height : map -> Units.dimension
  (** Get the distance between two cells. *)
  val distance : map -> Location.location -> Location.location -> Units.dimension
  (** Set the cell type at a location. *)
  val set_cell : map -> Location.location -> Cell.cell -> unit
  (** Get the cell type at a location. *)
  val get_cell : map -> Location.location -> Cell.cell
  (** Get the entities at a cell. *)
  val get_entities : map -> Location.location -> (Player.id, Entity.entity) Hashtbl.t
end = struct
  type map = {
    width : Units.dimension;
    height : Units.dimension;
    cells : Cell.cell array array;
    entities : (Player.id, Entity.entity) Hashtbl.t array array
  }
  let make width height = {
    width;
    height;
    cells=Array.make height (Array.make width (Cell.Normal 0));
    entities=Array.make height (Array.make width (Hashtbl.create 0))
  }
  let width {width} = width
  let height {height} = height
  let distance {width; height} (l1x, l1y) (l2x, l2y) =
    let x_dist = abs (l2x - l1x) in
    let y_dist = abs (l2y - l1y) in
    min x_dist (width - x_dist) + min y_dist (height - y_dist)
  let set_cell {cells} (x, y) cell = cells.(y).(x) <- cell
  let get_cell {cells} (x, y) = cells.(y).(x)
  let get_entities {entities} (x, y) = entities.(y).(x)
end

(** The game state. *)
module rec State : sig
  type state = {
    mutable turn_number : int;                      (** The current turn number *)
    player_id : int;                                (** The player's ID. *)
    players : (Player.id, Player.player) Hashtbl.t; (** The players in the game. *)
    map : Map.map                                   (** The game map. *)
  }
end = State

(** A command issued by a player. *)
module rec Command : sig
  (** The type of a command. *)
  type command =
    | Move of Location.location * Direction.direction (** A move command. *)
end = Command
