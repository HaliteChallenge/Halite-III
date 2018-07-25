open Halite
let name () = "MyBot"
let turn = function
  | 0 -> [Spawn 0]
  | _ ->
    let directions = [| North; South; East; West |] in
    let random_direction () = Array.get directions (Random.int (Array.length directions)) in
    List.map (fun e -> Move (e, random_direction ())) (Player.entities (Player.current ()))
