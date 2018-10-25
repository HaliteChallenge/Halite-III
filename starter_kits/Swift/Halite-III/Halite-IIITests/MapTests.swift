//
//  MapTests.swift
//  Halite-IIITests
//
//  Created by Chris Downie on 10/25/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import XCTest

class MapTests: XCTestCase {
    func testSubscript() {
        let map = Map(width: 5, height: 5)
        let middle = Position(x: 2, y: 2)
        
        let cell = map[middle]
        XCTAssert(cell.isEmpty)
    }
}

class MapCellTests: XCTestCase {
    let initialPosition = Position(x: 10, y: 10)
    
    func testIsEmptyWithEmptyCell() {
        let cell = MapCell(position: initialPosition, haliteAmount: 0, ship: nil, structure: nil)
        XCTAssert(cell.isEmpty)
    }
    
    func testIsEmptyWithShip() {
        let ship = Ship(owner: "123", id: "1", position: initialPosition, haliteAmount: 0)
        let cell = MapCell(position: initialPosition, haliteAmount: 0, ship: ship, structure: nil)
        XCTAssertFalse(cell.isEmpty)
    }
    
    func testIsEmptyWithStructure() {
        let structure = Dropoff(owner: "123", id: "1", position: initialPosition)
        let cell = MapCell(position: initialPosition, haliteAmount: 0, ship: nil, structure: structure)
        XCTAssertFalse(cell.isEmpty)
    }
}
