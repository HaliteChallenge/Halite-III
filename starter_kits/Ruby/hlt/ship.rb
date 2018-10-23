require 'entity'
require 'commands'
require 'constants'
require 'direction'
require 'position'


# Ship class to house ship entities
class Ship < Entity
  attr_reader :halite_amount

  def initialize(owner, id, position, halite_amount)
    super(owner, id, position)
    @halite_amount = halite_amount
  end

  # Is this ship at max halite capacity?
  def is_full?
    @halite_amount >= Constants::MAX_HALITE
  end

  # Return a move to transform this ship into a dropoff.
  def make_dropoff
    "#{Commands::CONSTRUCT} #{@id}"
  end

  # Return a move to move this ship in a direction without
  # checking for collisions.
  def move(direction)
    raw_direction = direction
    if !direction.is_a?(String) || !"nsewo".include?(direction)
      raw_direction = Direction.convert(direction)
    end
    "#{Commands::MOVE} #{@id} #{raw_direction}"
  end

  # Don't move this ship.
  def stay_still
    "#{Commands::MOVE} #{@id} #{Commands::STAY_STILL}"
  end

  # Creates an instance of a ship for a given player given the engine's input.
  # :param player_id: The id of the player who owns this ship
  # :param player_id: The id of the player who owns this ship
  # :return: The ship id and ship object
  def self.generate(game, player_id)
    id, x, y, halite = game.read_ints_from_input
    Ship.new(player_id, id, Position.new(x, y), halite)
  end

  def to_s
    "#{self.class}(id=#{@id}, #{@position}, cargo=#{@halite_amount} halite)"
  end
end
