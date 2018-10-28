
# Welcome to your first Halite-III bot!
#
# This bot's purpose is simple (don't expect it to win complex games!):
#  1. Initialize game
#  2. If a ship is full or the current ship position has less than 10% halite,
#     move in a random direction. Otherwise, collect halite.
#  3. Try to spawn a ship at the shipyard.

# Load the files we need
$:.unshift(File.dirname(__FILE__) + "/hlt")
require 'game'
require 'logger'

# Our bot is named...
bot_name = "MyRubyBot"

# Set up a logger
LOGGER =  Logger.new("#{bot_name}_#{Time.now.to_f}.log").tap do |l|
  l.formatter = proc do |severity, datetime, progname, msg|
    "#{datetime.strftime("%m-%d %H:%M:%S.%3N")} - #{severity} - #{msg}\n"
  end
end

######## Game Begin ########

# This game object contains the initial game state.
game = Game.new(bot_name)
# At this point "game" variable is populated with initial map data.
# This is a good place to do computationally expensive start-up pre-processing.

# As soon as you call "ready" function below, the 2 second per turn timer will
# start.
game.ready

# Now that your bot is initialized, save a message to yourself in the log file
#   with some important information.
#   Here, you log here your id, which you can always fetch from the game object
#   by using my_id.
LOGGER.info("Successfully created bot! My Player ID is #{game.my_id}.")

######## Game Loop ########

while true
  # This loop handles each turn of the game. The game object changes every turn,
  #   and you refresh that state by running update_frame().
  game.update_frame
  # Extract player metadata and the updated map metadata here for convenience.
  me = game.me
  map = game.map

  # A command queue holds all the commands you will run this turn.
  #   You build this list up and submit it at the end of the turn.
  command_queue = []

  for ship in me.ships
    # For each of your ships, move randomly if the ship is on a low halite
    #   location or the ship is full.
    #   Else, stay still & collect halite.
    if map[ship.position].halite_amount < Constants::MAX_HALITE / 10 || ship.is_full?
      command_queue << ship.move( Direction.all_cardinals.sample )
    else
      command_queue << ship.stay_still
    end
  end

  # If the game is in the first 200 turns and you have enough halite, spawn a
  #   ship.  Don't spawn a ship if you currently have a ship at port, though -
  #   the ships will collide.
  if game.turn_number <= 200 && me.halite_amount >= Constants::SHIP_COST &&
    !map[me.shipyard].is_occupied?
    command_queue << me.shipyard.spawn
  end

  # Send your moves back to the game environment, ending this turn.
  game.end_turn(command_queue)
end
