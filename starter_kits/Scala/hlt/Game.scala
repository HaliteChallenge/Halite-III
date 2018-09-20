package hlt

class Game(name: String) {
  System.out.println(name)

  // Read in game parameters
  Constants.populateConstants(Input.readLine)
  val input: Input = Input.readInput
  val numPlayers: Int = input.getInt
  val me: Player = players(myId.id)

  // Load players and game map
  Log.open(myId.id)
  var myId = new PlayerId(input.getInt)
  var players: List[Player] = List.tabulate(numPlayers)(_ => Player.initialize)
  var gameMap: GameMap = GameMap._generate
  var turnNumber = 0

  def updateFrame() = {
    turnNumber = Input.readInput.getInt
    Log.log("=============== TURN " + turnNumber + " ================")
    players.map({ player =>
      val input = Input.readInput
      // Variable isn't used in this implementation, but we still need to consume it
      val currentPlayerId = new PlayerId(input.getInt)
      val numShips = input.getInt
      val numDropoffs = input.getInt
      val halite = input.getInt
      player._update(numShips, numDropoffs, halite)
    })
    gameMap._update()
    players.foreach { player =>
      player.ships.values.foreach { ship =>
        gameMap.at(ship).markUnsafe(ship)
      }
      gameMap.at(player.shipyard).structure = player.shipyard
      player.dropoffs.values.foreach { dropoff =>
        gameMap.at(dropoff).structure = dropoff
      }
    }
  }

  def endTurn(commands: Iterable[Command]) = {
    commands
      .map(c => c.command + ' ')
      .foreach(System.out.print)
    System.out.println()
  }
}
