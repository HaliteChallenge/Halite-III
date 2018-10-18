import hlt;
import positionals : ALL_CARDINALS;

import std.format;
import std.conv : to;
import std.random : choice, rndGen, unpredictableSeed;

class MyBot : Bot
{
  this()
  {
    super("MyDlangBot");
  }

  override void precomputation(GameState game)
  {
    // This method will run before the game starts
  }

  override void turn(GameState game)
  {
    foreach (ship; game.me.ships.byValue()) {
      if (game.map.at(ship).halite < constants.MAX_HALITE / 10 || ship.is_full) {
        ship.move(ALL_CARDINALS[].choice());
      } else {
        ship.stay_still();
      }
    }

    if (game.turn <= 200 &&
        game.me.halite >= constants.SHIP_COST &&
        !game.map.at(game.me.shipyard).is_occupied) {
      game.me.shipyard.spawn();
    }
  }
}

void main(string[] args)
{
  auto seed = unpredictableSeed;
  if (args.length > 1) {
    seed = to!uint(args[1]);
  }
  rndGen().seed(seed);

  auto bot = new MyBot();

  log(format!"Successfully created bot! My Player ID is %d. Bot rng seed is %d."(bot.state.me.id, seed));

  bot.run();
}
