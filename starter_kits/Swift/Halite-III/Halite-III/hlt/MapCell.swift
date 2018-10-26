//
//  MapCell.swift
//  Halite-III
//
//  Created by Chris Downie on 10/24/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import Foundation

/// A map cell is an object representation of a cell on the game map.
struct MapCell {
    let position: Position
    let haliteAmount: Int
    let ship: Ship?
    let structure: Structure?
    
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
        // TODO: Implement this
        fatalError("Not implemented")
    }
}
