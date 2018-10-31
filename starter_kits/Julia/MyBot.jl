using Halite3
using Logging

# Initialize the game.
log, constants, game = init_game()
# log is a function which takes a string and allows you to save messages for yourself.
# log is not available to use inside /src/*jl instead use @info to log from inside Halite3 module.
# This is required because the regular STDOUT (print statements) are reserved for the engine-bot communication.

# At this point "game" variable is populated with initial map data.
# This is a good place to do computationally expensive start-up pre-processing.
# As soon as you call "start_game" function below, the 2 second per turn timer will start.

start_game("MyJuliaBot")
# Now that your bot is initialized, save a message to yourself in the log file with some important information.
#   Here, you log here your id, which you can always fetch from the game object by using game.id.
log("Successfully created bot! My Player ID is $(game.id)")

while true
    # This loop handles each turn of the game. The game object changes every turn, and you refresh that state by
    #   running update_frame!(game).
    update_frame!(game)
    log("=============== TURN $(game.turn_number) ================")
    # You extract player metadata and the updated map metadata here for convenience.
    me = game.me
    game_map = game.game_map

    # A command queue holds all the commands you will run this turn. You build this list up and submit it at the
    #   end of the turn.
    command_queue = Vector()

    for ship in get_ships(me)
        # For each of your ships, move randomly if the ship is on a low halite location or the ship is full.
        #   Else, collect halite.
        if game_map[ship.position].halite_amount < constants.MAX_HALITE / 10 || is_full(ship, constants.MAX_HALITE)
            push!(command_queue, move(ship, random_direction()))
        else
            push!(command_queue, stay_still(ship))
        end
    end

    # If the game is in the first 200 turns and you have enough halite, spawn a ship.
    # Don't spawn a ship if you currently have a ship at port, though - the ships will collide.
    if game.turn_number <= 200 && me.halite_amount >= constants.SHIP_COST && !is_occupied(game_map[me.shipyard.position])
        push!(command_queue, spawn(me.shipyard))
    end

    # Send your moves back to the game environment, ending this turn.
    end_turn(command_queue)
end