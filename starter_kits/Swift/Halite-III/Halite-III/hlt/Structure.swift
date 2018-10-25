//
//  Structure.swift
//  Halite-III
//
//  Created by Chris Downie on 10/25/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import Foundation

typealias StructureID = String
protocol Structure {
    var id: StructureID { get }
    var owner: Player.ID { get }
    var position: Position { get }
    var type: StructureType { get }
}

enum StructureType {
    case dropoff, shipyard
}

