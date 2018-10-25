using Halite3
using Logging

# Initialize the game.
constants, game = Halite3.init_game()
# Logging allows you to save messages for yourself. This is required because the regular STDOUT
#   (print statements) are reserved for the engine-bot communication.
io = open("bot-$(game.id).log", "w+")
logger = SimpleLogger(io)
global_logger(logger)
# At this point "game" variable is populated with initial map data.
# This is a good place to do computationally expensive start-up pre-processing.
# As soon as you call "start_game" function below, the 2 second per turn timer will start.

Halite3.start_game("MyJuliaBot")
# Now that your bot is initialized, save a message to yourself in the log file with some important information.
#   Here, you log here your id, which you can always fetch from the game object by using game.id.
@info "Successfully created bot! My Player ID is $(game.id)"

while true
    # This loop handles each turn of the game. The game object changes every turn, and you refresh that state by
    #   running update_frame!(game).
    Halite3.update_frame!(game)
    @info "=============== TURN $(game.turn_number) ================"
    # You extract player metadata and the updated map metadata here for convenience.
    me = game.me
    game_map = game.game_map

    # A command queue holds all the commands you will run this turn. You build this list up and submit it at the
    #   end of the turn.
    command_queue = Vector()

    for ship in Halite3.get_ships(me)
        # For each of your ships, move randomly if the ship is on a low halite location or the ship is full.
        #   Else, collect halite.
        if game_map.cells[ship.position].halite_amount < constants.MAX_HALITE / 10 || Halite3.is_full(ship, constants.MAX_HALITE)
            direction = Halite3.random_direction()
            push!(command_queue, Halite3.move(ship, direction))
        else
            push!(command_queue, Halite3.stay_still(ship))
        end
    end

    # If the game is in the first 200 turns and you have enough halite, spawn a ship.
    # Don't spawn a ship if you currently have a ship at port, though - the ships will collide.
    if game.turn_number <= 200 && me.halite_amount >= constants.SHIP_COST && !Halite3.is_occupied(game_map.cells[me.shipyard.position])
        push!(command_queue, Halite3.spawn(me.shipyard))
    end

    # Send your moves back to the game environment, ending this turn.
    Halite3.end_turn(command_queue)
    flush(io)
end
close(io)