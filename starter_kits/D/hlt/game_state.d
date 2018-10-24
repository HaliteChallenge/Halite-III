import entity;
import game_map;
import types;

struct GameState
{
  int turn = 0;
  Player[PlayerId] players;
  Player me;
  GameMap map;
}

class Player
{
  immutable PlayerId id;
  Shipyard shipyard;
  int halite = 0;
  Ship[EntityId] ships;
  Dropoff[EntityId] dropoffs;

  this(PlayerId id, Shipyard shipyard)
  {
    this.id = id;
    this.shipyard = shipyard;
  }
}
