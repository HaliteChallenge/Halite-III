require_relative 'hlt'
include InputReader

module GameMap
  # A cell on the game map.
  class MapCell
    attr_accessor :ship, :structure, :position, :halite_amount
    def initialize(position, halite_amount)
      @position = position
      @halite_amount = halite_amount
      @ship = nil
      @structure = nil
    end

    # Whether this cell has no ships or structures
    # @return [Bool]
    def is_empty?
      @ship.nil? && @structure.nil?
    end

    # Whether this cell has any ships
    # @return [Bool]
    def is_occupied?
      !@ship.nil?
    end

    # Whether this cell has any structures
    # @return [Bool]
    def has_structure?
      !@structure.nil?
    end

    # What is the structure type in this cell
    # @return [Class || nil]
    def structure_type
      @structure.nil? ? nil : @structure.class
    end

    # Mark this cell as unsafe (occupied) for navigation.
    # Use in conjunction with GameMap.naive_navigate.
    def mark_unsafe(ship)
      @ship = ship
    end

    def ==(other)
      @position = other.position
    end

    def !=(other)
      !self.==(other)
    end
  end

  # The game map.
  #
  # Can be indexed by a position, or by a contained entity.
  # Coordinates start at 0. Coordinates are normalized for you.
  class GameMap
    def initialize(cells, width, height)
      @width = width
      @height = height
      @cells = cells
    end

    # Getter for position object or entity objects within the game map
    # @param location: the position or entity to access in the map
    # @return [cell]
    def [](location)
      if location.instance_of? Positionals::Position
        return @cells[location.y][location.x]
      elsif location.is_a? Entity
        return @cells[location.position.y][location.position.x]
      end
      nil
    end

    # Compute the Manhattan distance between two locations.
    # Accounts for wrap-around.
    # @param source: The source from where to calculate
    # @param target: The target to where calculate
    # @return [Fixnum] The distance between these items
    def calculate_distance(source, target)
      source = normalize(source)
      target = normalize(target)
      resulting_position = (source - target).abs
      [resulting_position.x, @width - resulting_position.x].min + \
        [resulting_position.y, @height - resulting_position.y].min
    end

    # Normalized the position within the bounds of the toroidal map.
    # i.e.: Takes a point which may or may not be within width and
    # height bounds, and places it within those bounds considering
    # wraparound.
    # @param position: A position object.
    # @return: A normalized position object fitting within the bounds of the map
    def normalize(position)
      Positionals::Position.new(position.x % @width, position.y % @height)
    end

    # Return the Direction(s) to move closer to the target point, or empty
    # if the points are the same. This move mechanic does not account for collisions.
    # The multiple directions are if both directional movements are viable.
    # @param source: The starting position
    # @param destination: The destination towards which you wish to move your object.
    # @return [Directions] A list of valid (closest) Directions towards your target.
    def get_unsafe_moves(source, destination)
      source = normalize(source)
      destination = normalize(destination)
      possible_moves = []
      distance = (destination - source).abs
      y_cardinality, x_cardinality = get_target_direction(source, destination)

      if distance.x != 0
        x_move = distance.x < (@width / 2) ? x_cardinality : Positionals::Direction.invert(x_cardinality)
        possible_moves.append(x_move)
      end

      if distance.y != 0
        y_move = distance.y < (@height / 2) ? y_cardinality : Positionals::Direction.invert(y_cardinality)
        possible_moves.append(y_move)
      end
      possible_moves
    end

    # Returns a singular safe move towards the destination.
    # @param ship: The ship to move.
    # @param destination: Ending position
    # @return [Direction]
    def naive_navigate(ship, destination)
      # No need to normalize destination, since get_unsafe_moves
      # does that.
      get_unsafe_moves.each do |direction|
        target_pos = ship.position.directional_offset(direction)
        unless self[target_pos].is_occupied?
          self[target_pos].mark_unsafe(ship)
          return direction
        end
      end
      Positionals::Direction::STILL
    end

    # Returns where in the cardinality spectrum the target is from source.
    # e.g.: North, East; South, West; etc.
    # NOTE: Ignores toroid
    # @param source: The source position
    # @param target: The target position
    # @return [Direction, Direction] A tuple containing the target Direction.
    # A tuple item (or both) could be nil if within same coords
    def self.get_target_direction(source, target)
      ns = case
           when target.y > source.y
            Positionals::Direction::SOUTH
           when target.y < source.y
            Positionals::Direction::NORTH
           else
            nil
           end
      ew = case
           when target.x > source.x
            Positionals::Direction::EAST
           when target.x < source.x
            Positionals::Direction::WEST
           else
            nil
           end
      [ns, ew]
    end

    # Creates a map object from the input given by the game engine.
    # @return [Map]
    def self.generate
      map_width, map_height = read_input.split.map(&:to_i)
      game_map = Array.new(map_height) { Array.new(map_width)}
      (0...map_height).each do |y_position|
        cells = read_input.split
        (0...map_width).each do |x_position|
          pos = Positionals::Position.new(x_position, y_position)
          game_map[y_position][x_position] = MapCell.new(pos, cells[x_position].to_i)
        end
      end
      GameMap.new(game_map, map_width, map_height)
    end

    # Updates this map object from the input given by the game engine.
    # @return [Self]
    def update
      # Mark cells as safe for navigation (will re-mark unsafe cells later)
      (0...@height).each do |y|
        (0...@width).each do |x|
          self[Positionals::Position.new(x,y)].ship = nil
        end
      end

      (0...read_input.to_i).each do
        cell_x, cell_y, cell_energy = read_input.split.map(&:to_i)
        self[Positionals::Position.new(cell_x, cell_y)].halite_amount = cell_energy
      end
      self
    end
  end
end
