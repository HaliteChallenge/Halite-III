public import constants;
public import game_state : GameState;
public import std.experimental.logger;

import networking;
import std.format;
import std.stdio;

class Bot
{
  immutable string name;
  GameState state;

  void precomputation(GameState game) {}
  abstract void turn(GameState game);

  this(string name)
  {
    this.name = name;

    populateConstants();
    state = makeGameState();

    sharedLog = new FileLogger(format!"bot-%d_%s.log"(state.me.id, name));
  }

  final void run()
  {
    info(format!"== Initializing bot %s =="(name));
    precomputation(state);

    write(name);
    doneSending();

    for (;;) {
      state.update();
      info(format!"== Starting turn %d =="(state.turn));
      turn(state);
      doneSending();
    }
  }

  private void doneSending()
  {
    write('\n');
    stdout.flush();
  }
}
