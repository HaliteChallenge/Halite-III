package hlt

import java.util.*

class Game {
    var turnNumber: Int = 0
    val myId: PlayerId
    val players: ArrayList<Player> = ArrayList()
    val me: Player
    val gameMap: GameMap

    init {
        Constants.populateConstants(Input.readLine())

        val input = Input.readInput()
        val numPlayers = input.nextInt
        myId = PlayerId(input.nextInt)

        Log.open(myId.id)

        for (i in 0 until numPlayers) {
            players.add(Player._generate())
        }
        me = players.get(myId.id)
        gameMap = GameMap._generate()
    }

    fun ready(name: String) {
        System.out.println(name)
    }

    fun updateFrame() {
        turnNumber = Input.readInput().nextInt
        Log.log("=============== TURN $turnNumber ================")

        for (i in 0 until players.size) {
            val input = Input.readInput()

            val currentPlayerId = PlayerId(input.nextInt)
            val numShips = input.nextInt
            val numDropoffs = input.nextInt
            val halite = input.nextInt

            players.get(currentPlayerId.id)._update(numShips, numDropoffs, halite)
        }

        gameMap._update()

        for (player in players) {
            for (ship in player.ships.values) {
                gameMap.at(ship)?.markUnsafe(ship)
            }

            gameMap.at(player.shipyard)?.structure = player.shipyard

            for (dropOff in player.dropoffs.values) {
                gameMap.at(dropOff)?.structure = dropOff
            }
        }
    }

    fun endTurn(commands: Collection<Command>) {
        for (command in commands) {
            System.out.print(command.command)
            System.out.print(' ')
        }
        System.out.println()
    }
}
