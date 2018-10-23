using Halite3
using Logging

constants, game = Halite3.init_game()

io = open("bot-$(game.id).log", "w+")
logger = SimpleLogger(io)
global_logger(logger)

Halite3.start_game("MyJuliaBot")

@info "Successfully created bot! My Player ID is $(game.id)"

while true
    Halite3.update_frame!(game)
    @info "=============== TURN $(game.turn_number) ================"

    me = game.me
    game_map = game.game_map

    command_queue = Vector()

    for ship in Halite3.get_ships(me)
        if game_map.cells[ship.position].halite_amount < constants.MAX_HALITE / 10 || Halite3.is_full(ship)
            direction = rand(Halite3.Direction)
            push!(command_queue, Halite3.move(ship, direction))
        else
            push!(command_queue, Halite3.stay_still(ship))
        end
    end


    if game.turn_number <= 200 && me.halite_amount >= constants.SHIP_COST && !Halite3.is_occupied(game_map.cells[me.shipyard.position])
        push!(command_queue, Halite3.spawn(me.shipyard))
    end
    Halite3.end_turn(command_queue)
end
close(io)