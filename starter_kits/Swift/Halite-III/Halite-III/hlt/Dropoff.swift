//
//  Dropoff.swift
//  Halite-III
//
//  Created by Chris Downie on 10/24/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import Foundation

/// You create a dropoff at any location on the map by converting a ship. Ships can store halite at a dropoff point
/// just as they would at the shipyard. If two dropoffs are constructed in the same location, the engine returns an
/// error and the construct command fails. The player class has the methods to access dropoffs.
struct Dropoff: Placeable, Structure {
    typealias ID = Int
    let type = StructureType.dropoff
    
    let owner: Player.ID
    let id: ID
    let position: Position
}
