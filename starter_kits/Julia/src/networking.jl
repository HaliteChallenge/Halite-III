mutable struct Game
    turn_number::Int
    id::Int
    players::Dict{Int, Player}
    game_map::GameMap
    me::Player
end
function init_game()
    constants = load_constants(JSON.parse(readline(stdin)))
    num_players, my_id = parse.(Int, split(readline()))

    players = Dict(i => generate(i) for i=0:num_players-1)

    me = players[my_id]
    game_map = generate()
    constants, Game(0, my_id, players, game_map, me)
end

send_commands(commands::Vector) = println(join(commands, " "))
start_game(name::String) = send_commands([name])

function update_frame!(game::Game)
    game.turn_number = parse(Int, readline())
    for _ in keys(game.players)
        player, num_ships, num_dropoffs, halite = parse.(Int, split(readline()))
        game.players[player] = update!(game.players[player], num_ships, num_dropoffs, halite)
    end
    game.game_map = update!(game.game_map)
    for player in values(game.players)
        for ship in get_ships(player)
            mark_unsafe(game.game_map.cells[ship.position], ship)
        end
        game.game_map.cells[player.shipyard.position].structure = player.shipyard
        for dropoff in get_dropoffs(player)
            game.game_map.cells[dropoff.position].structure = dropoff
        end
    end
    game
end

end_turn(commands::Vector) = send_commands(commands)
