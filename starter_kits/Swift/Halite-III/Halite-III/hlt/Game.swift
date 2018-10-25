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
    
    /// A game of Halite III is initialized when each player sends a string name. Game forwards this to the engine,
    /// and launches the game.
    ///
    /// - Parameter botName: The name of the bot
    func ready(botName: String) {
        
    }
    
    /// The game loop sends the game state to the players and processes commands returned from the players. This
    /// repeats for each turn. Games last between 400 and 500 turns per game depending on map size. The game engine
    /// kills any bot that takes more than 2,000 milliseconds to process.
    ///
    /// Updates the game state, and returns nothing.
    func updateFrame() {
        
    }
    
    /// The command queue is a list of commands. The player’s code fills this list with commands and sends it to the
    /// game object, where it is sent to the game engine. The game engine kills any bot that attempts to issue
    /// multiple commands to one ship.
    ///
    /// - Parameter commands: The command queue to execute.
    func endTurn(commands: [Command]) {
        
    }
}
