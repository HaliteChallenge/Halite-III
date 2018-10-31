
import entity;
import positionals : Direction, Position;
import std.math : abs;

class MapCell
{
  Position position;
  uint halite;
  Ship ship;
  Entity structure;

  this(int x, int y, uint halite)
  {
    this.position = Position(x, y);
    this.halite = halite;
  }

  @property bool is_empty() const
  {
    return !ship && !structure;
  }

  @property bool is_occupied() const
  {
    return ship !is null;
  }

  @property bool has_structure() const
  {
    return structure !is null;
  }

  void mark_unsafe(Ship ship)
  {
    this.ship = ship;
  }

  @property override string toString() const
  {
    import std.string : format;
    return format("MapCell(%s, halite=%d)", position, halite);
  }
}

class GameMap
{
  immutable uint width;
  immutable uint height;
  private MapCell[] cells;

  this(int width, int height)
  {
    this.width = width;
    this.height = height;
    this.cells.length = width * height;
  }

  MapCell at(Position pos)
  {
    auto normalized = normalize(pos);
    return cells[normalized.y * width + normalized.x];
  }

  MapCell at(int x, int y)
  {
    return at(Position(x, y));
  }

  MapCell at(const(Entity) ent)
  {
    return at(ent.position);
  }

  int calculate_distance(Position source, Position target) const
  {
    source = normalize(source);
    target = normalize(target);

    immutable dx = abs(source.x - target.x);
    immutable dy = abs(source.y - target.y);

    import std.algorithm : min;
    return min(dx, width - dx) + min(dy, height - dy);
  }

  Position normalize(Position position) const
  {
    immutable x = ((position.x % width) + width) % width;
    immutable y = ((position.y % height) + height) % height;
    return Position(x, y);
  }

  Direction[] get_unsafe_moves(Position source, Position destination) const
  {
    source = normalize(source);
    destination = normalize(destination);

    immutable dx = abs(source.x - destination.x);
    immutable dy = abs(source.y - destination.y);
    immutable wrapped_dx = width - dx;
    immutable wrapped_dy = height - dy;

    Direction[] possible_moves;

    if (source.x < destination.x) {
      possible_moves ~= dx > wrapped_dx ? Direction.WEST : Direction.EAST;
    } else if (source.x > destination.x) {
      possible_moves ~= dx < wrapped_dx ? Direction.WEST : Direction.EAST;
    }

    if (source.y < destination.y) {
      possible_moves ~= dy > wrapped_dy ? Direction.NORTH : Direction.SOUTH;
    } else if (source.y > destination.y) {
      possible_moves ~= dy < wrapped_dy ? Direction.NORTH : Direction.SOUTH;
    }

    return possible_moves;
  }

  Direction naive_navigate(Ship ship, Position destination)
  {
    foreach (direction; get_unsafe_moves(ship.position, destination)) {
      auto cell = at(ship.position.directional_offset(direction));
      if (!cell.is_occupied) {
        cell.mark_unsafe(ship);
        return direction;
      }
    }

    return Direction.STILL;
  }

  void clearCollisionData()
  {
    foreach (cell; cells) {
      cell.ship = null;
    }
  }
}
