//
//  Shipyard.swift
//  Halite-III
//
//  Created by Chris Downie on 10/24/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import Foundation

struct Shipyard: Placeable, Structure {
    typealias ID = Int
    
    let type = StructureType.shipyard
    
    let owner: Player.ID
    let id: ID
    let position: Position
    
    /// Returns an engine command to generate a new ship.
    ///
    /// - Returns: An engine command to generate a new ship.
    func spawn() -> Command {
        return .generate
    }
}
