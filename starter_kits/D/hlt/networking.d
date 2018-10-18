
import entity;
import game_map;
import game_state;
import positionals : Position;
import types;

import std.exception : enforce;
import std.stdio;
import std.string;

private class TokenReader
{
  private string[] tokens;

  void read()
  {
    enforce(empty());
    tokens = split(readln());
  }

  auto get(T = int)()
  {
    import std.conv : to;
    auto res = to!T(tokens[0]);
    tokens = tokens[1..$];
    return res;
  }

  @property bool empty() const
  {
    return tokens.length == 0;
  }
}

private TokenReader tokens;
static this()
{
  tokens = new TokenReader();
}


GameState makeGameState()
{
  GameState state;

  tokens.read();
  auto num_players = tokens.get;
  auto my_id = tokens.get!PlayerId;

  for (auto i = 0; i < num_players; ++i) {
    auto player = makePlayer();
    state.players[player.id] = player;
  }
  state.me = state.players[my_id];
  state.map = makeMap();

  enforce(tokens.empty);

  return state;
}

private Player makePlayer()
{
  tokens.read();
  auto playerId = tokens.get!PlayerId;
  auto shipyard_x = tokens.get;
  auto shipyard_y = tokens.get;

  return new Player(playerId, new Shipyard(playerId, shipyard_x, shipyard_y));
}

private GameMap makeMap()
{
  tokens.read();
  const width = tokens.get;
  const height = tokens.get;

  auto map = new GameMap(width, height);

  for (auto y = 0; y < map.height; ++y) {
    tokens.read();
    for (auto x = 0; x < map.width; ++x) {
      map.at(x, y) = new MapCell(x, y, tokens.get);
    }
  }

  return map;
}

private Ship makeShip(PlayerId player_id)
{
  tokens.read();
  auto ship_id = tokens.get!EntityId;
  auto x = tokens.get;
  auto y = tokens.get;
  auto halite = tokens.get;

  return new Ship(ship_id, player_id, Position(x, y), halite);
}

private Dropoff makeDropoff(PlayerId player_id)
{
  tokens.read();
  auto dropoff_id = tokens.get!EntityId;
  auto x = tokens.get!int;
  auto y = tokens.get!int;

  return new Dropoff(dropoff_id, player_id, Position(x, y));
}


void update(ref GameState state)
{
  tokens.read();
  state.turn = tokens.get;

  for (auto i = 0; i < state.players.length; ++i) {
    tokens.read();
    state.players[tokens.get!PlayerId].update();
  }

  state.map.update(state.players.values);

  enforce(tokens.empty);
}

private void update(Player player)
{
  auto num_ships = tokens.get;
  auto num_dropoffs = tokens.get;
  player.halite = tokens.get;

  player.ships.clear();
  for (int i = 0; i < num_ships; ++i) {
    auto s = makeShip(player.id);
    player.ships[s.id] = s;
  }

  player.dropoffs.clear();
  for (int i = 0; i < num_dropoffs; ++i) {
    auto d = makeDropoff(player.id);
    player.dropoffs[d.id] = d;
  }
}

private void update(GameMap map, Player[] players)
{
  tokens.read();
  immutable update_count = tokens.get;

  for (auto i = 0; i < update_count; ++i) {
    tokens.read();
    const x = tokens.get;
    const y = tokens.get;
    const halite = tokens.get;
    map.at(x, y).halite = halite;
  }

  map.clearCollisionData();
  foreach (player; players) {
    foreach (ship; player.ships.byValue()) {
      map.at(ship).mark_unsafe(ship);
    }

    map.at(player.shipyard).structure = player.shipyard;
    foreach (dropoff; player.dropoffs.byValue()) {
      map.at(dropoff).structure = dropoff;
    }
  }
}

void populateConstants()
{
  import constants;
  import std.conv : to;
  import std.json : parseJSON, JSON_TYPE;

  auto j = parseJSON(readln());

  SHIP_COST = to!int(j["NEW_ENTITY_ENERGY_COST"].integer);
  DROPOFF_COST = to!int(j["DROPOFF_COST"].integer);
  MAX_HALITE = to!int(j["MAX_ENERGY"].integer);
  MAX_TURNS = to!int(j["MAX_TURNS"].integer);
  EXTRACT_RATIO = to!int(j["EXTRACT_RATIO"].integer);
  MOVE_COST_RATIO = to!int(j["MOVE_COST_RATIO"].integer);
  INSPIRATION_ENABLED = j["INSPIRATION_ENABLED"].type() == JSON_TYPE.TRUE;
  INSPIRATION_RADIUS = to!int(j["INSPIRATION_RADIUS"].integer);
  INSPIRATION_SHIP_COUNT = to!int(j["INSPIRATION_SHIP_COUNT"].integer);
  INSPIRED_EXTRACT_RATIO = to!int(j["INSPIRED_EXTRACT_RATIO"].integer);
  INSPIRED_BONUS_MULTIPLIER = j["INSPIRED_BONUS_MULTIPLIER"].floating;
  INSPIRED_MOVE_COST_RATIO = to!int(j["INSPIRED_MOVE_COST_RATIO"].integer);
}
