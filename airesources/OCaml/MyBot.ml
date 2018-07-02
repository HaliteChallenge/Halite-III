open Halite
open Networking

let () =
  (* Get the initial state *)
  let state = init_state () in

  (* Announce the bot name *)
  Printf.printf "MyBot %d\n" state.player_id;
  flush stdout;

  let rec loop () =
    (* Update the state for a turn *)
    update_state state;

    (* Get a random direction for each entity owned by the player *)
    let player = Hashtbl.find state.players state.player_id in
    let random_direction () =
      let open Direction in
      let directions = [|North; South; East; West|] in
      let n = Random.int (Array.length directions) in
      Array.get directions n in
    let commands = Hashtbl.fold (fun c _ cs -> Command.Move (c, random_direction ())::cs) player.entities [] in

    (* Send the commands *)
    send_commands commands;

    (* Start the next turn *)
    loop () in
  loop ()
