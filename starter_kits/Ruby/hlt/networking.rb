require 'logger'
# The game object holds all metadata pertinent to the game and all its contents
class Game
  attr_reader :logger, :my_id, :me, :game_map, :turn_number
  # Initiates a game object collecting all start-state instances for the contained items for pre-game.
  # Also sets up basic logging.
  def initialize
    @turn_number = 0

    raw_constants = read_input
    Constants.load_constants(raw_constants)

    num_players, @my_id = read_input.split.map(&:to_i)

    @logger = Logger.new("bot-#{@my_id}.log")
    logger.level = Logger::DEBUG

    @players = {}
    (0...num_players).each do |player|
      @players[player] = Player.generate
    end
    @me = @players[@my_id]
    @game_map = GameMap::GameMap.generate
  end

  # Indicate that your bot is ready to play.
  # @param name: The name of your bot
  def ready(name)
    send_commands([name])
  end

  # Updates the game object's state.
  # @returns [self]
  def update_frame
    @turn_number = read_input.to_i
    logger.info("=============== TURN #{@turn_number} ================")

    @players.each do
      player, num_ships, num_dropoffs, halite = read_input.split.map(&:to_i)
      @players[player].update(num_ships, num_dropoffs, halite)
    end

    @game_map.update

    # Mark cells with ships as unsafe for navigation
    @players.values.each do |player|
      player.get_ships.each do |ship|
        @game_map[ship.position].mark_unsafe(ship)
      end

      @game_map[player.shipyard.position].structure = player.shipyard
      player.get_dropoffs.each do |dropoff|
        @game_map[dropoff.position].structure = dropoff
      end
    end
  end

  # Method to send all commands to the game engine, effectively ending your turn.
  # @param commands: Array of commands to send to the engine
  # @return [self]
  def end_turn(commands)
    send_commands(commands)
  end

  # Sends a list of commands to the engine.
  # @param commands: The list of commands to send.
  # @return [self]
  def send_commands(commands)
    puts commands.join(" ")
    STDOUT.flush
  end
end
