//
//  Direction.swift
//  Halite-III
//
//  Created by Chris Downie on 10/24/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import Foundation

enum Direction: CaseIterable {
    case north, west, south, east
    case still
    
    /// Returns an array of all cardinal directions
    ///
    /// - Returns: An array of all cardinal directions
    static func getAllCardinals() -> [Direction] {
        return [.north, .west, .south, .east]
    }
    
    /// Returns a letter command from a direction
    ///
    /// - Parameter direction: The direction to convert to a
    /// - Returns: A command equivalent to the given direction
    static func convert(_ direction: Direction, forShipId shipId: Ship.ID) -> Command {
        return .move(shipId: shipId, direction: direction)
    }
    
    /// Returns the opposite cardinal direction given a direction
    ///
    /// - Parameter direction: The input direction
    /// - Returns: The opposite direction
    static func invert(_ direction: Direction) -> Direction {
        switch direction {
        case .north:
            return .south
        case .south:
            return .north
        case .east:
            return .west
        case .west:
            return .east
        case .still:
            return .still
        }
    }
}

// MARK: - Convenience accessors
// This doesn't match the letter of the API, but it feels like a more Swift-friendly means of providing the same
// functionality.
extension Direction {
    static prefix func -(_ direction: Direction) -> Direction {
        return Direction.invert(direction)
    }
    
    static var allCardinals = {
        return Direction.getAllCardinals()
    }()
}

