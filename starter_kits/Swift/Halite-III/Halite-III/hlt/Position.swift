//
//  Position.swift
//  Halite-III
//
//  Created by Chris Downie on 10/24/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import Foundation

struct Position {
    let x: Int
    let y: Int
    
    /// Returns a new position based on moving one unit in the given direction from the given position.
    ///
    /// This method takes a direction such as Direction.west or an equivalent tuple such as (0, -1), but will not
    /// work with commands such as "w".
    ///
    /// - Parameter direction: The direction to move this position
    /// - Returns: The position moved one unit from the given direction
    func directionalOffset(_ direction: Direction) -> Position {
        // TODO
        return Position(x:0, y: 0)
    }
    
    /// This is a convenience method for directionalOffset(:) that takes a tuple instead of a direction.
    ///
    /// - Parameter tuple: An (x,y) tuple to offset the current position. It should represent one square in any
    ///   cardinal direction
    /// - Returns: The position moved one unit from the given position.
    func directionalOffset(tuple: (Int, Int)) -> Position {
        // TODO
        return Position(x: tuple.0, y: tuple.1)
    }
    
    /// Returns a list of all positions around the given position in each cardinal direction.
    ///
    /// - Returns: A list of all positions around the given position in each cardinal direction.
    func getSurroundingCardinals() -> [Position] {
        return []
    }
}
