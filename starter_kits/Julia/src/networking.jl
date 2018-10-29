"The game object holds all metadata pertinent to the game and all its contents."
mutable struct Game
    turn_number::Int
    id::Int
    players::Dict{Int, Player}
    game_map::GameMap
    me::Player
end

"Initiates a game object collecting all start-state instances for the contained items for pre-game."
function init_game()
    constants = load_constants(JSON.parse(readline(stdin)))
    num_players, my_id = parse.(Int, split(readline()))
    io = open("bot-$(my_id).log", "w+")
    logger = Logging.SimpleLogger(io)
    Logging.global_logger(logger)
    log(s::String, io=io) = (@info s; flush(io))
    players = Dict(i => generate_players() for i=0:num_players-1)
    me = players[my_id]
    game_map = generate_game_map()
    log, constants, Game(0, my_id, players, game_map, me)
end

"Sends a list of commands to the engine."
send_commands(commands::Vector) = println(join(commands, " "))

"""
Indicate that your bot is ready to play.
# Arguments
- `name::String` : The name of your bot.
"""
start_game(name::String) = send_commands([name])

"Updates the game object's state."
function update_frame!(game::Game)
    game.turn_number = parse(Int, readline())
    for _ in keys(game.players)
        player, num_ships, num_dropoffs, halite = parse.(Int, split(readline()))
        update_player!(game.players[player], num_ships, num_dropoffs, halite)
    end
    update_game_map!(game.game_map)
    for player in values(game.players)
        for ship in get_ships(player)
            mark_unsafe!(game.game_map[ship.position], ship)
        end
        game.game_map[player.shipyard.position].structure = player.shipyard
        for dropoff in get_dropoffs(player)
            game.game_map[dropoff.position].structure = dropoff
        end
    end
    game
end

"Method to send all commands to the game engine, effectively ending your turn."
end_turn(commands::Vector) = send_commands(commands)
