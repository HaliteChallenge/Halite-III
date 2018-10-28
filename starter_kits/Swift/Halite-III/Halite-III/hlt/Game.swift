//
//  Game.swift
//  Halite-III
//
//  Created by Chris Downie on 10/24/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import Foundation

/// The game object holds all metadata to run the game, and is an organizing layer between your code and the game
/// engine. Game initializes the game, which includes generating the map and registering the players.
struct Game {
    var turnNumber = 0
    
    var me: Player {
        return players[myId]
    }
    var gameMap: Map
    
    private var myId: Int
    private var players: [Player]
    private let shouldStripNewlines = false
    private let networking: Networking
    
    init() {
        let networking = Networking()
        self.networking = networking
        
        // Load shared constants from the first line of input
        let constantInput = networking.readConstants()
        do {
            try Constant.loadShared(with: constantInput.json)
        } catch {
            fatalError("Failed to parse constants from input")
        }
        
        // Load player count and ID from next line of input.
        let identity = networking.readIdentity()
        myId = identity.myId
        
        _ = Log.seedShared(playerId: myId)
        
        // Load each player
        players = (0..<identity.numPlayers).map { index in
            let playerInput = networking.readPlayer()
            Log.shared.info("Got player: \(playerInput)")
            let shipyard = Shipyard(owner: playerInput.playerId, id: unknownStructureId, position: playerInput.shipyardPosition)
            return Player(id: playerInput.playerId, shipyard: shipyard)
        }
        
        // Load map size
        let mapSize = networking.readMapDimensions()
        Log.shared.info("Got map size: \(mapSize)")
        let initialHalite = (0..<mapSize.mapHeight).map { row -> [Int] in
            let mapRow = networking.readMapRow()
            Log.shared.info("Got map row: \(mapRow)")
            return mapRow.haliteAmount
        }

        gameMap = Map(width: mapSize.mapWidth, height: mapSize.mapHeight, initialHalite: initialHalite)
    }
    
    /// A game of Halite III is initialized when each player sends a string name. Game forwards this to the engine,
    /// and launches the game.
    ///
    /// - Parameter botName: The name of the bot
    func ready(botName: String) {
        Log.shared.debug("Sending ready command with name \(botName)")
        networking.writeReady(botName: botName)
    }
    
    /// The game loop sends the game state to the players and processes commands returned from the players. This
    /// repeats for each turn. Games last between 400 and 500 turns per game depending on map size. The game engine
    /// kills any bot that takes more than 2,000 milliseconds to process.
    ///
    /// Updates the game state, and returns nothing.
    mutating func updateFrame() {
        turnNumber = networking.readTurnNumber()
        
        Log.shared.info("============ TURN \(turnNumber) ============")
        
        let playerUpdates = (0..<players.count).map { _ -> Networking.PlayerUpdate in
            let playerUpdate = networking.readPlayerUpdate()
            Log.shared.debug("Got update for player \(playerUpdate.playerID): \(playerUpdate.ships.count) ships, \(playerUpdate.dropoffs.count) dropoffs.")
            return playerUpdate
        }
        let mapUpdates = networking.readMapUpdates()
        
        apply(playerUpdates: playerUpdates, mapUpdates: mapUpdates)
    }
    
    /// The command queue is a list of commands. The player’s code fills this list with commands and sends it to the
    /// game object, where it is sent to the game engine. The game engine kills any bot that attempts to issue
    /// multiple commands to one ship.
    ///
    /// - Parameter commands: The command queue to execute.
    func endTurn(commands: [Command]) {
        Log.shared.debug("Ending turn with \(commands.count) commands")
        networking.write(commands: commands)
    }
    
    // MARK: - Private methods
    mutating func apply(playerUpdates: [Networking.PlayerUpdate], mapUpdates: [Networking.MapUpdate]) {
        Log.shared.debug("Applying \(playerUpdates.count) player updates and \(mapUpdates.count) map updates.")
        
        // Update Players
        var updatedPlayers = [Player]()
        playerUpdates.forEach { update in
            guard let playerToUpdate = players.first(where: { $0.id == update.playerID }) else {
                fatalError("Can't fild player with id \(update.playerID) to update.")
            }
            let updatedPlayer = Player(id: update.playerID,
                                       shipyard: playerToUpdate.shipyard,
                                       ships: update.ships,
                                       dropoffs: update.dropoffs)
            updatedPlayers.append(updatedPlayer)
        }
        players = updatedPlayers
        
        // Update Map
        var haliteGrid = gameMap.haliteGrid
        mapUpdates.forEach { update in
            haliteGrid[update.position.y][update.position.x] = update.haliteAmount
        }
        let newMap = Map(width: gameMap.width, height: gameMap.height, initialHalite: haliteGrid)

        players.forEach { player in
            player.getShips().forEach { ship in
                newMap[ship.position].markUnsafe(ship: ship)
            }
            newMap[player.shipyard.position].structure = player.shipyard
            
            player.getDropoffs().forEach { dropoff in
                newMap[dropoff.position].structure = dropoff
            }
        }
        gameMap = newMap
    }
}
