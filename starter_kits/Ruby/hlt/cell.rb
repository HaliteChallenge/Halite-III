
# A cell on the game map.
class Cell
  attr_reader :position
  attr_accessor :structure, :halite_amount, :ship

  def initialize(position, halite_amount)
    @position = position
    @halite_amount = halite_amount
    @ship = nil
    @structure = nil
  end

  # :return: Whether this cell has no ships or structures
  def is_empty?
    return @ship == nil && @structure == nil
  end

  # :return: Whether this cell has any ships
  def is_occupied?
    return @ship != nil
  end

  # :return: Whether this cell has any structures
  def has_structure?
    return @structure != nil
  end

  # :return: What is the structure type in this cell
  def structure_type
    return !@structure ? nil : @structure.class
  end

  # Mark this cell as unsafe (occupied) for navigation.
  # Use in conjunction with GameMap.naive_navigate.
  def mark_unsafe(ship)
    @ship = ship
  end

  def ==(other)
    other.nil? ? false : @position == other.position
  end

  def to_s
    return "Cell(#{@position}, halite=#{@halite_amount})"
  end
end
