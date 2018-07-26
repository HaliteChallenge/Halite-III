open Halite
open Networking
open Bot

let () =
  let state = get_init () in
  send_init (name state);
  while true do
    let turn_number = get_frame state in
    end_turn (turn turn_number);
  done
