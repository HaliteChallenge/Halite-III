require 'direction'

class Position
  attr_reader :x, :y

  def initialize(x, y)
    @x = x
    @y = y
  end

  # Returns the position considering a Direction cardinal tuple
  # :param direction: the direction cardinal array
  # :return: a new position moved in that direction
  def directional_offset(direction)
    return self + Position.new(*direction)
  end

  # :return: Returns a list of all positions around this specific position in
  #   each cardinal direction
  def get_surrounding_cardinals
    Direction.all_cardinals.map do |direction|
      directional_offset(direction)
    end
  end

  def +(other)
    Position.new(@x + other.x, @y + other.y)
  end

  def -(other)
    Position.new(@x - other.x, @y - other.y)
  end

  def abs
    Position.new(@x.abs, @y.abs)
  end

  def ==(other)
    return other.nil? ? false : @x == other.x && @y == other.y
  end

  def to_s
    "Position (#{@x}, #{@y})"
  end
end
