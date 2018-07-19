open Halite
open Networking
open Bot

let () =
  let state = init_state () in
  send_name (name state);
  update_state state;
  init state;
  while true do
    update_state state;
    send_commands (turn state);
  done
