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
    
    private var cells: [[Cell]]
    var haliteGrid: [[Int]] {
        return cells.map { $0.map { $0.haliteAmount } }
    }
    
    init(width: Int, height: Int, initialHalite: [[Int]]? = nil) {
        self.width = width
        self.height = height
        self.cells = (0 ..< height).map { row in
            (0 ..< width).map { column in
                Cell(position: Position(x: column, y: row),
                     haliteAmount: initialHalite?[row][column] ?? 0,
                     ship: nil,
                     structure: nil)
            }
        }
    }
    
    /// A map cell is an object representation of a cell on the game map.
    class Cell: Placeable {
        let position: Position
        let haliteAmount: Int
        var ship: Ship?
        var structure: Structure?
        
        init(position: Position, haliteAmount: Int, ship: Ship?, structure: Structure?) {
            self.position = position
            self.haliteAmount = haliteAmount
            self.ship = ship
            self.structure = structure
        }
        
        /// Returns True if the cell is empty.
        var isEmpty: Bool {
            return ship == nil && structure == nil
        }
        
        /// Returns True if there is a ship on this cell.
        var isOccupied: Bool {
            return ship != nil
        }
        
        /// Returns True if there is a structure (a dropoff or shipyard) on this cell.
        var hasStructure: Bool {
            return structure != nil
        }
        
        /// Returns the type of structure on this cell, or nil if there is no structure.
        var structureType: StructureType? {
            return structure?.type
        }
        
        /// Used to mark the cell under this ship as unsafe (occupied) for collision avoidance.
        ///
        /// This marking resets every turn and is used by naive_navigate to avoid collisions.
        ///
        /// - Parameter ship: The ship on this cell.
        func markUnsafe(ship: Ship) {
            self.ship = ship
        }
    }
    
    /// A method that computes the Manhattan distance between two locations, and accounts for the toroidal wraparound.
    ///
    /// - Parameters:
    ///   - source: The position to measure from
    ///   - target: The position to measure to
    /// - Returns: The distance between the source and the target
    func calculateDistance(source: Position, target: Position) -> Int {
        let from = normalize(position: source)
        let to = normalize(position: target)
        let horizontalDistance = abs(from.x - to.x)
        let verticalDistance = abs(from.y - to.y)
        
        return min(horizontalDistance, width - horizontalDistance)
            + min(verticalDistance, height - verticalDistance)
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
        var x = position.x
        var y = position.y
        
        // In Swift, % is a remainder operator, not modulus. So we have to get a positive number to get the wrap
        // around to work correctly.
        while x < 0 {
            x += width
        }
        while y < 0 {
            y += height
        }
        
        return Position(x: x % width, y: y % height)
    }
    
    /// A method that returns a list of direction(s) to move closer to a target disregarding collision possibilities.
    /// Returns an empty list if the source and destination are the same.
    ///
    /// - Parameters:
    ///   - source: The position to move from
    ///   - destination: The position to move to
    /// - Returns: List of closest directions toward the given target.
    func getUnsafeMoves(source: Position, destination: Position) -> [Direction] {
        let from = normalize(position: source)
        let to = normalize(position: destination)
        let xDistance = to.x - from.x
        let yDistance = to.y - from.y
        let xCardinality: Direction = xDistance > 0 ? .east : .west
        let yCardinality: Direction = yDistance > 0 ? .south : .north
        
        var possibleMoves = [Direction]()
        
        if xDistance != 0 {
            if Double(abs(xDistance)) < Double(width) / 2.0 {
                possibleMoves.append(xCardinality)
            } else {
                possibleMoves.append(Direction.invert(xCardinality))
            }
        }
        
        if yDistance != 0 {
            if Double(abs(yDistance)) < Double(width) / 2.0 {
                possibleMoves.append(yCardinality)
            } else {
                possibleMoves.append(Direction.invert(yCardinality))
            }
        }
        
        return possibleMoves
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
        // No need to normalize destination, since getUnsafeMoves does that
        let direction = getUnsafeMoves(source: ship.position, destination: destination).first { direction in
            let targetPosition = ship.position.directionalOffset(direction)
            return !self[targetPosition].isOccupied
        }
        
        if let direction = direction {
            let cell = self[ship.position.directionalOffset(direction)]
            cell.markUnsafe(ship: ship)
        }
        
        return direction ?? .still
    }
    
    /// Index the game map and find a particular map cell with game_map[position].
    ///
    /// - Parameter position: The position to return a map cell
    subscript(_ position: Position) -> Map.Cell {
        get {
            let normalized = normalize(position: position)
            return cells[normalized.y][normalized.x]
        }
    }
    
    // Mark: - Convenience methods
    subscript(_ placeable: Placeable) -> Map.Cell {
        get {
            return self[placeable.position]
        }
    }
    
    func unsafeMoves(from: Position, to: Position) -> [Direction] {
        return getUnsafeMoves(source: from, destination: to)
    }
}

extension Map {
    /// Computes the Manhattan distance between two locations, and accounts for the toroidal wraparound.
    ///
    /// This is a convenience method on calculateDistance(source:target) the fits the Swift language better
    /// - Parameters:
    ///   - from: The position to measure from
    ///   - to: The position to measure to
    /// - Returns: The manhattan distance between those two positions
    func distance(from: Position, to: Position) -> Int {
        return calculateDistance(source: from, target: to)
    }
}
