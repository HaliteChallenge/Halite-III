//
//  Command.swift
//  Halite-III
//
//  Created by Chris Downie on 10/24/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import Foundation

enum Command {
    case move(shipId: Ship.ID, direction: Direction)
    case construct(fromShipId: Ship.ID)
    case generate
}
