open Halite

let init_state () : State.t =
  let (num_players, player_id) = Scanf.scanf "%d %d\n" (fun x y -> (x, Player.Player y)) in
  let players : (Player.id, Player.t) Hashtbl.t = Hashtbl.create num_players in
  for _ = 1 to num_players do
    Scanf.scanf "%d %d %d\n" begin
      fun player x y ->
        let id = Player.Player player in
        Hashtbl.add players id {id; factory=(x, y); dropoffs=[]; energy=0; entities=[]}
    end
  done;
  let (map_w, map_h) = Scanf.scanf "%d %d\n" (fun x y -> (x, y)) in
  let map = Map.make map_w map_h in
  for y = 0 to map_h - 1 do
    for x = 0 to map_w - 1 do
      Scanf.scanf "%d%c" (fun energy _ -> let cell = Map.get_cell map (x, y) in cell.energy <- energy);
    done;
    Scanf.scanf "\n" ()
  done;
  {turn_number=0; player_id; players; entities=Hashtbl.create 0; map}

let update_state (state : State.t) =
  state.turn_number <- Scanf.scanf "%d\n" (fun x -> x);
  for _ = 1 to Hashtbl.length state.players do
    Scanf.scanf "%d %d %d %d\n" begin
      fun player_id num_entities num_dropoffs energy ->
        let player_id = Player.Player player_id in
        let player = Hashtbl.find state.players player_id in
        player.energy <- energy;
        player.entities <- [];
        player.dropoffs <- [];
        for _ = 1 to num_entities do
          Scanf.scanf "%d %d %d %d\n" begin
            fun entity x y energy ->
              let id = Entity.Entity entity in begin
                match Hashtbl.find_opt state.entities id with
                | Some entity -> entity.location <- (x, y); entity.energy <- energy
                | None -> Hashtbl.add state.entities id {id; location=(x, y); energy}
              end;
              player.entities <- id::player.entities
          end
        done;
        for _ = 1 to num_dropoffs do
          Scanf.scanf "%d %d %d\n" begin
            fun dropoff x y ->
              player.dropoffs <- {id=Dropoff dropoff; location=(x, y)}::player.dropoffs
          end
        done
    end
  done;
  Scanf.scanf "%d\n" begin
    fun num_changed_cells ->
      for _ = 1 to num_changed_cells do
        Scanf.scanf "%d %d %d\n" begin
          fun x y energy -> let cell = Map.get_cell state.map (x, y) in cell.energy <- energy
        end
      done
  end

let send_commands commands =
  List.iter begin
    function
    | (Command.Move (Entity.Entity entity, dir)) ->
      Printf.printf "m %d %s "
        entity (match dir with North -> "n" | South -> "s" | East -> "e" | West -> "w")
    | (Command.Spawn energy) -> Printf.printf "g %d " energy
    | (Command.Dump (Entity.Entity entity, energy)) -> Printf.printf "d %d %d " entity energy
    | (Command.Construct (Entity.Entity entity)) -> Printf.printf "c %d " entity
  end commands;
  Printf.printf "\n"; flush stdout

let send_name name = Printf.printf "%s\n" name; flush stdout
