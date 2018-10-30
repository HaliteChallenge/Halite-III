//
//  Ship.swift
//  Halite-III
//
//  Created by Chris Downie on 10/24/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import Foundation

struct Ship: Placeable {
    typealias ID = Int
    
    let owner: Player.ID
    let id: ID
    let position: Position
    let haliteAmount: Int
    
    /// Cargo: returns a boolean True if ship is carrying 1,000 halite (the maximum). Otherwise returns False.
    ///
    /// - Returns: True if ship is carrying 1,000 halite (the maximum). Otherwise returns False.
    func isFull() -> Bool {
        return haliteAmount >= Constant.shared.MAX_HALITE
    }
    
    /// Returns an engine command to convert this ship into a dropoff.
    ///
    /// Ships can be converted into dropoff sites at their present location. The conversion costs 4,000 halite,
    /// deducted from total current stored halite. The converted ship’s halite cargo and the halite in the sea under
    /// the new dropoff is credited to the player. These credits resolve first, and can be used toward the cost of
    /// the dropoff.
    ///
    /// - Returns: An engine command to convert this ship into a dropoff.
    func makeDropoff() -> Command {
        return .construct(fromShipId: id)
    }
    
    /// Returns an engine command to keep this ship where it is and collect halite.
    ///
    /// Ships can collect 25% of the halite from the sea at their present location, rounded up to the nearest integer.
    ///
    /// - Returns: An engine command to keep this ship where it is and collect halite.
    func stayStill() -> Command {
        return .move(shipId: id, direction: .still)
    }
    
    /// Returns an engine command to move this ship in a direction without checking for collisions.
    ///
    /// Ships can move one square in a cardinal direction per turn. Each move costs 10% of the halite available in
    /// the sea at the ship’s starting location, debited from the ships’ cargo. The direction of the move is
    /// communicated via the command queue.
    ///
    /// - Parameter direction: The direction to move
    /// - Returns: An engine command to move this ship in a direction without checking for collisions.
    func move(direction: Direction) -> Command {
        return .move(shipId: id, direction: direction)
    }
}
