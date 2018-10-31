require 'logger'
require 'json'
require 'constants'
require 'map'
require 'player'

# The game object holds all metadata pertinent to the game and all its contents
class Game
  attr_reader :map, :me, :turn_number, :my_id

  # Initiates a game object collecting all start-state instances for the contained items for pre-game.
  # Also sets up basic logging.
  def initialize(name)
    @name = name
    # implicit IO flush
    $stdout.sync = true

    # Grab constants JSON
    Constants.load_constants(JSON.parse(read_from_input))

    @turn_number = 0
    @num_players, @my_id = read_ints_from_input

    @players = {}
    @num_players.times do
      player_id, player = Player.generate(read_ints_from_input)
      @players[player_id] = player
    end

    @me = @players[@my_id]
    @map = Map.generate(self)
  end

  # Indicate that your bot is ready to play.
  def ready
    end_turn([@name])
  end

  # Updates the game object's state.
  # :returns: nothing.
  def update_frame
    @turn_number = read_ints_from_input.first
    LOGGER.info("=============== TURN #{@turn_number} ================")

    @num_players.times do |_|
      player, num_ships, num_dropoffs, halite = read_ints_from_input
      @players[player].update(self, num_ships, num_dropoffs, halite)
    end

    @map.update(self)


    # Mark cells with ships as unsafe for navigation
    for player in @players.values
      for ship in player.ships
        @map[ship.position].mark_unsafe(ship)
      end

      @map[player.shipyard.position].structure = player.shipyard
      for dropoff in player.dropoffs
        @map[dropoff.position].structure = dropoff
      end
    end
  end

  def end_turn(commands)
    write_to_output(commands.join(" "))
  end

  def read_from_input
    $stdin.gets.strip
  end

  def read_ints_from_input
    read_from_input.split(' ').map { |v| Integer(v) }
  end

  private

  def write_to_output(data)
    data = "#{data.strip}"
    LOGGER.info("Sending: #{data.inspect}")
    $stdout.puts(data)
  end
end
