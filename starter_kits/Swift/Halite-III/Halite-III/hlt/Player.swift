//
//  Player.swift
//  Halite-III
//
//  Created by Chris Downie on 10/24/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import Foundation

struct Player {
    typealias ID = Int
    
    let id: ID
    let shipyard: Shipyard
    let haliteAmount: Int = 0
    
    // MARK: Ships
    /// Returns the ship object associated with the ship id provided as an argument.
    ///
    /// - Parameter ship: The ship ID to query for.
    /// - Returns: The ship object associated with the ship ID provided as an argument.
    func get(ship: Ship.ID) -> Ship? {
        // TODO: Implement this
        return nil
    }
    
    /// Returns a list of all ship objects.
    ///
    /// - Returns: All ship objects.
    func getShips() -> [Ship] {
        // TODO: Implement this
        return []
    }
    
    /// Checks if you have a ship with this id.
    ///
    /// - Parameter ship: The ship ID to query for.
    /// - Returns: True if you have a ship with this id.
    func has(ship: Ship.ID) -> Bool {
        // TODO: Implement this
        return false
    }
    
    // MARK: Dropoffs
    /// Returns the dropoff object associated with the dropoff id provided as an argument.
    ///
    /// - Parameter dropoff: The dropoff ID to query for
    /// - Returns: The dropoff object associated with the dropoff id provided as an argument.
    func get(dropoff: Dropoff.ID) -> Dropoff? {
        // TODO: Implement this
        return nil
    }

    /// Returns a list of all dropoff objects.
    ///
    /// - Returns: All dropoff objects.
    func getDropoffs() -> [Dropoff] {
        // TODO: Implement this
        return []
    }
}
