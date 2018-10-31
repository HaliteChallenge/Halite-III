//
//  Structure.swift
//  Halite-III
//
//  Created by Chris Downie on 10/25/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import Foundation

protocol Placeable {
    var position: Position { get }
}

typealias StructureID = Int
let unknownStructureId = -1
protocol Structure {
    var id: StructureID { get }
    var owner: Player.ID { get }
    var type: StructureType { get }
}

enum StructureType {
    case dropoff, shipyard
}

