require_relative 'commands'
module Positionals
  # Holds positional tuples in relation to cardinal directions
  class Direction
    NORTH = [ 0, -1]
    SOUTH = [ 0,  1]
    EAST  = [ 1,  0]
    WEST  = [-1,  0]

    STILL = [ 0,  0]

    # Returns all contained items in each cardinal
    # @return [Array] An array of all cardinals
    def self.get_all_cardinals
      [
        Direction::NORTH,
        Direction::SOUTH,
        Direction::EAST,
        Direction::WEST
      ]
    end

    # Converts from this direction tuple notation to the
    # engine's string notation.
    # @param direction: The direction in this notation.
    # @return [String] The character equivalent for the game engine.
    def self.convert(direction)
      case direction
      when Direction::NORTH
        Commands::NORTH
      when Direction::SOUTH
        Commands::SOUTH
      when Direction::EAST
        Commands::EAST
      when Direction::WEST
        Commands::WEST
      when Direction::STILL
        Commands::STAY_STILL
      else
        raise "Invalid direction tuple"
      end
    end

    # Returns the opposite cardinal direction from a given direction.
    # @param direction: The input direction
    # @return [Direction] The opposite direction.
    def self.invert
      case direction
      when Direction::NORTH
        Direction::SOUTH
      when Direction::SOUTH
        Direction::NORTH
      when Direction::EAST
        Direction::WEST
      when Direction::WEST
        Direction::EAST
      when Direction::STILL
        Direction::STILL
      else
        raise "Invalid direction"
      end
    end
  end

  class Position
    attr_reader :x, :y

    def initialize(x, y)
      @x = x
      @y = y
    end

    # Returns the position considering a Direction cardinal tuple
    # @param direction: The direction cardinal tuple
    # @return [Position] A new position moved in that direction
    def directional_offset(direction)
      self + new(*direction)
    end

    # @return Returns a list of all positions around this specific
    # position in each cardinal direction.
    def get_surrounding_cardinals
      Direction.get_all_cardinals.map do |direction|
        directional_offset(direction)
      end
    end

    def +(other)
      new(@x + other.x, @y + other.y)
    end

    def -(other)
      new(@x - other.x, @y - other.y)
    end

    def abs
      new(@x.abs, @y.abs)
    end

    def ==(other)
      @x == other.x && @y == other.y
    end

    def !=(other)
      !self.==(other)
    end
  end
end
