#!/usr/bin/env ruby

require_relative 'hlt/hlt'
DIRECTION = Positionals::Direction
# This game object contains the initial game state.
game = Game.new
# At this point "game" variable is populated with initial map data.
# This is a good place to do computationally expensive start-up pre-processing.
# As soon as you call "ready" function below, the 2 second per turn timer will start.
game.ready("MyRubyBot")

# Now that your bot is initialized, save a message to yourself in the log file with
# some important information. Here, you log here your id, which you can always fetch
# from the game object by using my_id.
game.logger.info("Successfully created bot! My Player ID is #{game.my_id}")

# <<< Game Loop >>>
while true
  # This loop handles each turn of the game. The game object changes every turn, and you refresh that state by
  # running update_frame().
  game.update_frame

  # You extract player metadata and the updated map metadata here for convenience.
  me = game.me
  game_map = game.game_map

  # A command queue holds all the commands you will run this turn. You build this
  # list up and submit it at the end of the turn.
  command_queue = []

  me.get_ships.each do |ship|
    # For each of your ships, move randomly if the ship is on a low halite location or the ship is full.
    # Else, collect halite.
    if game_map[ship.position].halite_amount < (Constants::MAX_HALITE / 10) || ship.is_full
      command_queue.push(
          ship.move([ DIRECTION::NORTH, DIRECTION::SOUTH, DIRECTION::EAST, DIRECTION::WEST ].sample))
    else
      command_queue.push(ship.stay_still)
    end
  end

  # If the game is in the first 200 turns and you have enough halite, spawn a ship.
  # Don't spawn a ship if you currently have a ship at port, though - the ships will collide.
  if game.turn_number <= 200 && me.halite_amount >= Constants::SHIP_COST && !game_map[me.shipyard].is_occupied?
    command_queue.push(me.shipyard.spawn)
  end

  # Send your moves back to the game environment, ending this turn.
  game.end_turn(command_queue)
end
