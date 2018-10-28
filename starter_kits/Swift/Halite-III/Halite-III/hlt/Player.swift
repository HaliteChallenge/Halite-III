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
    let haliteAmount: Int
    
    let ships: [Ship]
    let dropoffs: [Dropoff]
    
    // MARK: Initialization
    init(id: ID, shipyard: Shipyard, haliteAmount: Int = 0, ships: [Ship] = [], dropoffs: [Dropoff] = []) {
        self.id = id
        self.shipyard = shipyard
        self.haliteAmount = haliteAmount
        self.ships = ships
        self.dropoffs = dropoffs
    }

    // MARK: Ships
    /// Returns the ship object associated with the ship id provided as an argument.
    ///
    /// - Parameter ship: The ship ID to query for.
    /// - Returns: The ship object associated with the ship ID provided as an argument.
    func get(ship id: Ship.ID) -> Ship? {
        return ships.first(where: { ship in
            ship.id == id
        })
    }
    
    /// Returns a list of all ship objects.
    ///
    /// - Returns: All ship objects.
    func getShips() -> [Ship] {
        return ships
    }
    
    /// Checks if you have a ship with this id.
    ///
    /// - Parameter ship: The ship ID to query for.
    /// - Returns: True if you have a ship with this id.
    func has(ship id: Ship.ID) -> Bool {
        return ships.contains { $0.id == id }
    }
    
    // MARK: Dropoffs
    /// Returns the dropoff object associated with the dropoff id provided as an argument.
    ///
    /// - Parameter dropoff: The dropoff ID to query for
    /// - Returns: The dropoff object associated with the dropoff id provided as an argument.
    func get(dropoff id: Dropoff.ID) -> Dropoff? {
        return dropoffs.first(where: { $0.id == id })
    }

    /// Returns a list of all dropoff objects.
    ///
    /// - Returns: All dropoff objects.
    func getDropoffs() -> [Dropoff] {
        return dropoffs
    }
}
