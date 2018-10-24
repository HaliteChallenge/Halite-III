require 'commands'

# Holds positional arrays in relation to cardinal directions
module Direction
  NORTH = [0, -1]
  SOUTH = [0, 1]
  EAST = [1, 0]
  WEST = [-1, 0]

  STILL = [0, 0]

  # Returns all contained items in each cardinal
  # :return: An array of cardinals
  def self.all_cardinals
    return [NORTH, SOUTH, EAST, WEST]
  end

  # Converts from this direction tuple notation to the engine's string notation
  # :param direction: the direction in this notation
  # :return: The character equivalent for the game engine
  def self.convert(direction)
    case direction
    when NORTH
      return Commands::NORTH
    when SOUTH
      return Commands::SOUTH
    when EAST
      return Commands::EAST
    when WEST
      return Commands::WEST
    when STILL
      return Commands::STAY_STILL
    else
      raise IndexError
    end
  end

  # Returns the opposite cardinal direction given a direction
  # :param direction: The input direction
  # :return: The opposite direction
  def self.invert(direction)
    case direction
    when NORTH
      return SOUTH
    when SOUTH
      return NORTH
    when EAST
      return WEST
    when WEST
      return EAST
    when STILL
      return STILL
    else
      raise IndexError
    end
  end
end
