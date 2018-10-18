require_relative 'input_reader'
include InputReader

# Base Entity Class from whence Ships, Dropoffs and Shipyards inherit
class Entity
  attr_accessor :owner, :id, :position

  def initialize(owner, id, position)
    @owner = owner
    @id = id
    @position = position
  end

  # Method which creates an entity for a specific player given input from the engine.
  # @param player_id: The player id for the player who owns this entity
  # @return [id, Entity]: An instance of Entity along with its id
  def self.generate(player_id)
    ship_id, x_position, y_position = read_input.split.map(&:to_i)
    position = Positionals::Position.new(x_position, y_position)
    [ship_id, new(player_id, ship_id, position)]
  end
end

# Dropoff class for housing dropoffs
class Dropoff < Entity ; end

# Shipyard class to house shipyards
class Shipyard < Entity
  # Return a move to spawn a new ship.
  def spawn
    Commands::GENERATE
  end
end

# Ship class to house ship entities
class Ship < Entity
  def initialize(owner, id, position, halite_amount)
    super(owner, id, position)
    @halite_amount = halite_amount
  end

  # Is this ship at max capacity?
  def is_full
    @halite_amount >= Constants::MAX_HALITE
  end

  # Return a move to transform this ship into a dropoff.
  def make_dropoff
    "#{Commands::CONSTRUCT} #{@id}"
  end

  # Return a move to move this ship in a direction without checking for collisions.
  def move(direction)
    raw_direction = direction

    unless direction.is_a?(String) || ['n','s','e','w','o'].include?(direction)
      raw_direction = Positionals::Direction.convert(direction)
    end

    "#{Commands::MOVE} #{@id} #{raw_direction}"
  end

  # Don't move this ship
  def stay_still
    "#{Commands::MOVE} #{@id} #{Commands::STAY_STILL}"
  end

  # Creates an instance of a ship for a given player given the engine's input.
  # @param player_id: The id of the player who owns this ship
  # @return [ship_id, Ship] The ship id and ship object
  def self.generate(player_id)
    ship_id, x_position, y_position, halite = read_input.split.map(&:to_i)
    position = Positionals::Position.new(x_position, y_position)
    [ship_id, new(player_id, ship_id, position, halite)]
  end
end
