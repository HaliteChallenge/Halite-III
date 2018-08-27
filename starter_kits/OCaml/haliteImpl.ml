include Units

module rec Dropoff : sig
  type id = Dropoff of int
  exception Invalid_id
  type t = {
    id : id;
    owner : Player.t;
    cell : Cell.t
  }
  val dropoffs : (id, t) Hashtbl.t
  val all_dropoffs : t list ref
  val id : t -> id
  val of_id : id -> t
  val all : unit -> t list
  val location : t -> location
  val owner : t -> Player.t
  val cell : t -> Cell.t
  val energy : t -> energy
end = struct
  type id = Dropoff of int
  exception Invalid_id
  type t = {
    id : id;
    owner : Player.t;
    cell : Cell.t
  }
  let dropoffs : (id, t) Hashtbl.t = Hashtbl.create 0
  let all_dropoffs : t list ref = ref []
  let id {id} = id
  let of_id id =
    try
      Hashtbl.find dropoffs id
    with Not_found -> raise Invalid_id
  let all () = !all_dropoffs
  let location {cell} = Cell.location cell
  let owner {owner} = owner
  let cell {cell} = cell
  let energy {cell} = Cell.energy cell
end

and Entity : sig
  type id = Entity of int
  exception Invalid_id
  type t = {
    id : id;
    mutable energy : energy;
    owner : Player.t;
    mutable cell : Cell.t
  }
  val entities : (id, t) Hashtbl.t
  val all_entities : t list ref
  val id : t -> id
  val of_id : id -> t
  val all : unit -> t list
  val location : t -> location
  val energy : t -> energy
  val owner : t -> Player.t
  val cell : t -> Cell.t
end = struct
  type id = Entity of int
  exception Invalid_id
  type t = {
    id : id;
    mutable energy : energy;
    owner : Player.t;
    mutable cell : Cell.t
  }
  let entities : (id, t) Hashtbl.t = Hashtbl.create 0
  let all_entities : t list ref = ref []
  let id {id} = id
  let of_id id =
    try
      Hashtbl.find entities id
    with Not_found -> raise Invalid_id
  let all () = !all_entities
  let location {cell} = Cell.location cell
  let energy {energy} = energy
  let owner {owner} = owner
  let cell {cell} = cell
end

and Player : sig
  type id = Player of int
  exception Invalid_id
  type t = {
    id : id;
    factory_cell : Cell.t;
    mutable dropoffs : Dropoff.t list;
    mutable energy : energy;
    mutable entities : Entity.t list
  }
  val players : (id, t) Hashtbl.t
  val all_players : t list ref
  val current_player : t option ref
  val id : t -> id
  val of_id : id -> t
  val current : unit -> t
  val all : unit -> t list
  val factory : t -> location
  val factory_cell : t -> Cell.t
  val energy : t -> energy
  val dropoffs : t -> Dropoff.t list
  val entities : t -> Entity.t list
end = struct
  type id = Player of int
  exception Invalid_id
  type t = {
    id : id;
    factory_cell : Cell.t;
    mutable dropoffs : Dropoff.t list;
    mutable energy : energy;
    mutable entities : Entity.t list
  }
  let players : (id, t) Hashtbl.t = Hashtbl.create 0
  let all_players : t list ref = ref []
  let current_player : t option ref = ref None
  let id {id} = id
  let of_id id =
    try
      Hashtbl.find players id
    with Not_found -> raise Invalid_id
  let current () =
    match !current_player with
      Some p -> p
    | None -> failwith "Players not initialized"
  let all () = !all_players
  let factory {factory_cell} = Cell.location factory_cell
  let factory_cell {factory_cell} = factory_cell
  let energy {energy} = energy
  let dropoffs {dropoffs} = dropoffs
  let entities {entities} = entities
end

and Cell : sig
  type t = {
    location : location;
    mutable energy : energy;
    mutable entity : Entity.t option;
    mutable owner : Player.t option;
    mutable dropoff : Dropoff.t option
  }
  val location : t -> location
  val energy : t -> energy
  val entity : t -> Entity.t option
  val owner : t -> Player.t option
  val dropoff : t -> Dropoff.t option
end = struct
  type t = {
    location : location;
    mutable energy : energy;
    mutable entity : Entity.t option;
    mutable owner : Player.t option;
    mutable dropoff : Dropoff.t option
  }
  let location {location} = location
  let energy {energy} = energy
  let entity {entity} = entity
  let owner {owner} = owner
  let dropoff {dropoff} = dropoff
end

and Board : sig
  type t = {
    width : dimension;
    height : dimension;
    cells : Cell.t array array;
  }
  val board : Board.t option ref
  val width : unit -> dimension
  val height : unit -> dimension
  val at : location -> Cell.t
end = struct
  type t = {
    width : dimension;
    height : dimension;
    cells : Cell.t array array;
  }
  let board : Board.t option ref = ref None
  let get () =
    match !board with
      Some b -> b
    | None -> failwith "Board not initialized"
  let width () = (get ()).width
  let height () = (get ()).height
  let at {x; y} = (get ()).cells.(y).(x)
end

module Constants = struct
  let entity_cost = 500
  let dropoff_cost = 2000
  let move_cost_ratio = 10
  let mine_ratio = 4
  let max_capacity = 1000
end
