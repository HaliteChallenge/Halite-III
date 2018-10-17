require 'position'

# Base Entity Class from whence Ships, Dropoffs and Shipyards inherit
class Entity
  attr_reader :owner, :id, :position

  def initialize(owner, id, position)
    @owner = owner
    @id = id
    @position = position
  end

  # Method which creates an entity for a specific player given input from the engine.
  # :param game: The game object for fetching information
  # :param player_id: The player id for the player who owns this entity
  # :return: An instance of Entity along with its id
  def self.generate(game, player_id)
    id, x, y = game.read_ints_from_input
    self.new(player_id, id, Position.new(x, y))
  end

  def to_s
    "#{self.class}(id=#{@id}, #{@position})"
  end
end
