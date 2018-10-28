import Foundation

//This game object contains the initial game state.
var game = Game()

//
// At this point "game" variable is populated with initial map data. This is a good place to do computationally
// expensive start-up pre-processing. As soon as you call "ready" function below, the 2 second per turn timer
// will start.
//

game.ready(botName: "MySwiftBot")

Log.shared.info("Successfully created bot! My Player ID is \(game.me.id)")

while true {
    // This loop handles each turn of the game. The game object changes every turn, and you refresh that state by
    // running updateFrame().
    game.updateFrame()
    
    // You extract player metadata and the updated map metadata here for convenience.
    let me = game.me
    let gameMap = game.gameMap
    
    // A command queue holds all the commands you will run this turn. You build this list up and submit it at the
    // end of the turn.
    var commandQueue = [Command]()
    
    for ship in me.getShips() {
        // For each of your ships, move randomly if the ship is on a low halite location or the ship is full. Else,
        // collect halite.
        if gameMap[ship.position].haliteAmount < Constant.shared.MAX_HALITE / 10 || ship.isFull() {
            commandQueue.append(ship.move(direction: Direction.allCardinals.randomElement()!))
        } else {
            commandQueue.append(ship.stayStill())
        }
    }
    
    // If the game is in the first 200 turns and you have enough halite, spawn a ship. Don't spawn a ship if you
    // currently have a ship at port, though - the ships will collide.
    if game.turnNumber <= 200,
        me.haliteAmount >= Constant.shared.SHIP_COST,
        !gameMap[me.shipyard.position].isOccupied {
        commandQueue.append(me.shipyard.spawn())
    }
    
    // Send your moves back to the game environment, ending this turn.
    game.endTurn(commands: commandQueue)
}

