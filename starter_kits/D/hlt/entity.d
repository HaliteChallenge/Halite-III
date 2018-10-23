
import constants;
import positionals : Position, Direction;
import types;

import std.stdio : write;
import std.format;

abstract class Entity
{
  immutable EntityId id;
  immutable PlayerId ownerId;
  immutable Position position;

  this(EntityId id, PlayerId ownerId, Position position)
  {
    this.id = id;
    this.ownerId = ownerId;
    this.position = position;
  }
}

class Dropoff : Entity
{
  this(EntityId id, PlayerId ownerId, Position position)
  {
    super(id, ownerId, position);
  }
}

class Shipyard : Entity
{
  this(PlayerId owner, int x, int y)
  {
    super(-1, owner, Position(x, y));
  }

  final void spawn() const
  {
    write("g ");
  }
}

class Ship : Entity
{
  int halite;

  this(PlayerId player_id, EntityId ship_id, Position position, int halite)
  {
    super(player_id, ship_id, position);
    this.halite = halite;
  }

  @property bool is_full() const
  {
    return halite >= constants.MAX_HALITE;
  }


  final void make_dropoff() const
  {
    write(format!"c %d "(id));
  }

  final void move(Direction direction) const
  {
    write(format!"m %d %c "(id, direction));
  }

  final void stay_still() const
  {
    write(format!"m %d %c "(id, Direction.STILL));
  }
}
