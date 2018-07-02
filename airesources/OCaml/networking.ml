open Halite

let init_state () : State.state =
  let (num_players, player_id) = Scanf.scanf "%d %d\n" (fun x y -> (x, y)) in
  let players : (Player.id, Player.player) Hashtbl.t = Hashtbl.create num_players in
  let rec loop n =
    if n = 0 then () else
      let (id, factory_location) = Scanf.scanf "%d %d %d\n" (fun x y z -> (x, (y, z))) in
      Hashtbl.add players id {id; energy=0; factory_location; entities=Hashtbl.create 10};
      loop (n - 1) in
  loop num_players;
  let (map_w, map_h) = Scanf.scanf "%d %d\n" (fun x y -> (x, y)) in
  let map = Map.make map_w map_h in
  let rec loop y =
    if y = map_h then () else begin
      let rec loop' x =
        if x = map_w then () else begin
          begin
            match Scanf.scanf "%c " (fun x -> x) with
            | 'f' -> Map.set_cell map (x, y) Cell.Factory
            | _ -> Map.set_cell map (x, y) (Cell.Normal (Scanf.scanf "%d\n" (fun x -> x)))
          end; loop' (x + 1)
        end in
      loop' 0;
      loop (y + 1)
    end in
  loop 0;
  {turn_number=0; player_id; players; map}

let update_state (state : State.state) =
  state.turn_number <- Scanf.scanf "%d\n" (fun x -> x);
  let rec loop n =
    if n = 0 then () else begin
      let (player_id, num_entities, energy) = Scanf.scanf "%d %d %d\n" (fun x y z -> (x, y, z)) in
      let player = Hashtbl.find state.players player_id in
      player.energy <- energy;
      Hashtbl.clear player.entities;
      let rec loop' n =
        if n = 0 then () else begin
          let (location, energy) = Scanf.scanf "%d %d %d %d\n" (fun x y _ z -> ((x, y), z)) in
          Hashtbl.add player.entities location {owner=player_id; location; energy};
          loop' (n - 1) end in
      loop' num_entities;
      loop (n - 1)
    end in
  loop (Hashtbl.length state.players)

let send_commands commands =
  List.iter (fun (Command.Move ((x, y), dir)) ->
      Printf.printf "m %d %d %s "
        x y (match dir with North -> "n" | South -> "s" | East -> "e" | West -> "w")) commands;
  Printf.printf "\n"; flush stdout
