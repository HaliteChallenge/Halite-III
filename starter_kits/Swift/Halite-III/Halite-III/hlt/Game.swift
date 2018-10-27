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
class Game {
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
        networking.writeReady(botName: botName)
    }
    
    /// The game loop sends the game state to the players and processes commands returned from the players. This
    /// repeats for each turn. Games last between 400 and 500 turns per game depending on map size. The game engine
    /// kills any bot that takes more than 2,000 milliseconds to process.
    ///
    /// Updates the game state, and returns nothing.
    func updateFrame() {
        // TODO: Implement this.
    }
    
    /// The command queue is a list of commands. The player’s code fills this list with commands and sends it to the
    /// game object, where it is sent to the game engine. The game engine kills any bot that attempts to issue
    /// multiple commands to one ship.
    ///
    /// - Parameter commands: The command queue to execute.
    func endTurn(commands: [Command]) {
        networking.write(commands: commands)
    }
}
