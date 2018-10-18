require 'direction'
require 'position'
require 'cell'


# The game map.
# Can be indexed by a position, or by a contained entity.
# Coordinates start at 0. Coordinates are normalized for you.
class Map

  def initialize(cells, width, height)
    @width = width
    @height = height
    @cells = cells
  end

  # Getter for position object or entity objects within the game map
  # :param location: the position or entity to access in this map
  # :return: the contents housing that cell or entity
  def [](location)
    if location.is_a? Position
      location = normalize(location)
      return @cells[location.y][location.x]
    elsif location.is_a? Entity
      return @cells[location.position.y][location.position.x]
    end
    return nil
  end

  # Compute the Manhattan distance between two locations.
  #   Accounts for wrap-around.
  # :param source: The source from where to calculate
  # :param target: The target to where calculate
  # :return: The distance between these items
  def calculate_distance(source, target)
    source = normalize(source)
    target = normalize(target)
    resulting_position = (source - target).abs
    return [resulting_position.x, @width - resulting_position.x].min +
      [resulting_position.y, @height - resulting_position.y].min
  end

  # Normalize the position within the bounds of the toroidal map.
  #   i.e.: Takes a point which may or may not be within width and
  #   height bounds, and places it within those bounds considering
  #   wraparound.
  # :param position: A position object.
  # :return: A normalized position object fitting within the bounds of the map
  def normalize(position)
    Position.new(position.x % @width, position.y % @height)
  end

  # Return the Direction(s) to move closer to the target point, or empty if the
  #   points are the same.
  #   This move mechanic does not account for collisions. The multiple
  #   directions are if both directional movements are viable.
  # :param source: The starting position
  # :param destination: The destination towards which you wish to move your object.
  # :return: A list of valid (closest) Directions towards your target.
  def get_unsafe_moves(source, destination)
    source = normalize(source)
    destination = normalize(destination)
    possible_moves = []
    distance = (destination - source).abs
    y_cardinality, x_cardinality = self.class.get_target_direction(source, destination)

    if distance.x != 0
      possible_moves << (distance.x < (@width / 2) ? x_cardinality :
                  Direction.invert(x_cardinality))
    end
    if distance.y != 0
      possible_moves << (distance.y < (@height / 2) ? y_cardinality :
                  Direction.invert(y_cardinality))
    end
    return possible_moves
  end

  # Returns a singular safe move towards the destination.
  # :param ship: The ship to move.
  # :param destination: Ending position
  # :return: A direction.
  def naive_navigate(ship, destination)
    # No need to normalize destination, since get_unsafe_moves does that
    for direction in get_unsafe_moves(ship.position, destination)
      target_pos = ship.position.directional_offset(direction)
      if !self[target_pos].is_occupied?
        self[target_pos].mark_unsafe(ship)
        return direction
      end
    end

    return Direction::STILL
  end

  # Updates this map object from the input given by the game engine
  # :return: nothing
  def update(game)
    # Mark cells as safe for navigation (will re-mark unsafe cells later)
    for y in 0...@height
      for x in 0...@width
        self[Position.new(x, y)].ship = nil
      end
    end

    game.read_ints_from_input.first.times do
      cell_x, cell_y, cell_energy = game.read_ints_from_input
      self[Position.new(cell_x, cell_y)].halite_amount = cell_energy
    end
  end

  # Creates a map object from the input given by the game engine
  # :return: The map object
  def self.generate(game)
    map_width, map_height = game.read_ints_from_input
    game_map = []
    for y_pos in 0...map_height
      game_map[y_pos] = []
      cells = game.read_ints_from_input
      for x_pos in 0...map_width
        game_map[y_pos][x_pos] = Cell.new(Position.new(x_pos, y_pos), cells[x_pos])
      end
    end
    return Map.new(game_map, map_width, map_height)
  end

  # Returns where in the cardinality spectrum the target is from source.
  # e.g.: North, East; South, West; etc.
  # NOTE: Ignores toroid
  # :param source: The source position
  # :param target: The target position
  # :return: An array containing the target Direction(s). Either item (or both)
  #   could be nil if within same coords
  def self.get_target_direction(source, target)
    y_cardinality = target.y > source.y ? Direction::SOUTH : (target.y < source.y ? Direction::NORTH : nil)
    x_cardinality = target.x > source.x ? Direction::EAST  : (target.x < source.x ? Direction::WEST : nil)
    return [y_cardinality, x_cardinality]
  end
end
