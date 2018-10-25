//
//  Map.swift
//  Halite-III
//
//  Created by Chris Downie on 10/24/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import Foundation

/// Gameplay takes place on a wrapping rectangular grid 32x32, 40x40, 48x48, 56x56, or 64x64 in dimension. The map
/// edges wrap to their opposite edge and create a torus shape. The game map can be indexed by a position or by a
/// contained entity (ship, shipyard, or dropoff). The game map has width and height as member variables.
struct Map {
    let width: Int
    let height: Int
    
    /// A method that computes the Manhattan distance between two locations, and accounts for the toroidal wraparound.
    ///
    /// - Parameters:
    ///   - source: The position to measure from
    ///   - target: The position to measure to
    /// - Returns: The distance between the source and the target
    func calculateDistance(source: Position, target: Position) -> Int {
        return 0
    }
    
    /// A method that normalizes a position within the bounds of the toroidal map.
    ///
    /// Useful for handling the wraparound modulus arithmetic on x and y. For example, if a ship at (x = 31, y = 4)
    /// moves to the east on a 32x32 map, the normalized position would be (x = 0, y = 4), rather than the off-the-map
    /// position of (x = 32, y = 4).
    ///
    /// - Parameter position: The position to normalize
    /// - Returns: The normalized position.
    func normalize(position: Position) -> Position {
        return position
    }
    
    /// A method that returns a list of direction(s) to move closer to a target disregarding collision possibilities.
    /// Returns an empty list if the source and destination are the same.
    ///
    /// - Parameters:
    ///   - source: The position to move from
    ///   - destination: The position to move to
    /// - Returns: List of closest directions toward the given target.
    func getUnsafeMoves(source: Position, destination: Position) -> [Direction] {
        return []
    }
    
    /// A method that returns a direction to move closer to a target without colliding with other entities.
    ///
    /// Returns a direction of “still” if no such move exists.
    ///
    /// - Parameters:
    ///   - ship: The ship attempting to navigate
    ///   - destination: The ship's desired position.
    /// - Returns: A single valid direction toward a given target.
    func naiveNavigate(ship: Ship, destination: Position) -> Direction {
        return .still
    }
    
    /// Index the game map and find a particular map cell with game_map[position].
    ///
    /// - Parameter position: The position to return a map cell
    subscript(_ position: Position) -> MapCell {
        get {
            return MapCell(position: position, haliteAmount: 0, ship: nil, structure: "what")
        }
    }
}
