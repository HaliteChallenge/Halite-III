open HaliteImpl

let get_init () =
  let (num_players, player_id) = Scanf.scanf "%d %d\n" (fun x y -> (x, Player.Player y)) in
  let players : (Player.id, location) Hashtbl.t = Hashtbl.create num_players in
  for _ = 1 to num_players do
    Scanf.scanf "%d %d %d\n" (fun player x y -> Hashtbl.add players (Player.Player player) {x; y})
  done;
  let (width, height) = Scanf.scanf "%d %d\n" (fun x y -> (x, y)) in
  let cells =
    Array.init height (fun y ->
        Array.init width (fun x ->
            Scanf.scanf "%d%c" (fun energy _ ->
                if x = width - 1 then Scanf.scanf "\n" ();
                ({location={x; y}; energy; entity=None; owner=None; dropoff=None} : Cell.t)
              )
          )
      ) in
  Board.board := Some {width; height; cells};
  Hashtbl.iter (fun id {x; y} ->
      let player : Player.t = {id; factory_cell=cells.(y).(x); dropoffs=[]; energy=0; entities=[]} in
      Hashtbl.add Player.players id player;
      Player.all_players := player::!Player.all_players;
    ) players;
  Player.current_player := Some (Hashtbl.find Player.players player_id)

let get_frame () =
  let turn_number = Scanf.scanf "%d\n" (fun x -> x) in
  let players = Player.players in
  for _ = 1 to Hashtbl.length players do
    Scanf.scanf "%d %d %d %d\n" begin
      fun player_id num_entities num_dropoffs energy ->
        let owner = Hashtbl.find players (Player.Player player_id) in
        owner.energy <- energy;
        owner.entities <- List.init num_entities (fun _ ->
            Scanf.scanf "%d %d %d %d\n" begin
              fun entity x y energy ->
                let id = Entity.Entity entity in
                let cell = Board.at {x; y} in
                match Hashtbl.find_opt Entity.entities id with
                | Some entity ->
                  entity.cell <- cell;
                  entity.energy <- energy;
                  entity
                | None ->
                  let entity : Entity.t = {id; energy; owner; cell} in
                  Hashtbl.add Entity.entities id entity;
                  entity
            end
          );
        owner.dropoffs <- List.init num_dropoffs (fun _ ->
            Scanf.scanf "%d %d %d\n" begin
              fun dropoff x y ->
                let id = Dropoff.Dropoff dropoff in
                match Hashtbl.find_opt Dropoff.dropoffs id with
                | Some dropoff -> dropoff
                | None ->
                  let dropoff : Dropoff.t = {id; owner; cell=Board.at {x; y}} in
                  Hashtbl.add Dropoff.dropoffs id dropoff;
                  dropoff
            end
          )
    end
  done;
  Scanf.scanf "%d\n" begin
    fun num_changed_cells ->
      for _ = 1 to num_changed_cells do
        Scanf.scanf "%d %d %d\n" begin
          fun x y energy -> (Board.at {x; y}).energy <- energy
        end
      done
  end;
  turn_number

let end_turn commands =
  let open Halite in
  let id entity = match Entity.id entity with Entity.Entity id -> id in
  List.iter begin
    function
    | Move (entity, dir) ->
      Printf.printf "m %d %s "
        (id entity)
        (match dir with North -> "n" | South -> "s" | East -> "e" | West -> "w")
    | Spawn energy -> Printf.printf "g %d " energy
    | Dump (entity, energy) -> Printf.printf "d %d %d " (id entity) energy
    | Construct entity -> Printf.printf "c %d " (id entity)
  end commands;
  Printf.printf "\n"; flush stdout

let send_init name = Printf.printf "%s\n" name; flush stdout
