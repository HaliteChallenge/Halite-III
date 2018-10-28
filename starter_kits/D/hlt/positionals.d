
enum Direction : char {
  NORTH = 'n',
  SOUTH = 's',
  EAST = 'e',
  WEST = 'w',
  STILL = 'o'
}

immutable Direction[4] ALL_CARDINALS = [
  Direction.NORTH,
  Direction.SOUTH,
  Direction.EAST,
  Direction.WEST
];

struct Position
{
  int x;
  int y;

  @property string toString() const
  {
    import std.string : format;
    return format("Position(%d, %d)", x, y);
  }

  Position directional_offset(Direction d) const
  {
    auto dx = 0;
    auto dy = 0;
    final switch (d) {
    case Direction.NORTH:
      dy = -1;
      break;
    case Direction.SOUTH:
      dy = 1;
      break;
    case Direction.EAST:
      dx = 1;
      break;
    case Direction.WEST:
      dx = -1;
      break;
    case Direction.STILL:
      // No move
      break;
    }
    return Position(x + dx, y + dy);
  }

  Position[] get_surrounding_cardinals() const
  {
    import std.array;
    import std.algorithm.iteration : map;
    return
      ALL_CARDINALS[]
      .map!(d => this.directional_offset(d))
      .array;
  }
}
