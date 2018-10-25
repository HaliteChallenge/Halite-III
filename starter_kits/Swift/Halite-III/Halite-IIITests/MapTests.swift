//
//  MapTests.swift
//  Halite-IIITests
//
//  Created by Chris Downie on 10/25/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import XCTest

class MapTests: XCTestCase {
    let tinyMap = Map(width: 5, height: 5)
    
    func testSubscript() {
        let middle = Position(x: 2, y: 2)
        
        let cell = tinyMap[middle]
        XCTAssert(cell.isEmpty)
    }
    
    func testNormalizeWithNormalPosition() {
        let position = Position(x: 2, y: 2)
        let normalizedPosition = tinyMap.normalize(position: position)
        XCTAssertEqual(normalizedPosition, position)
    }
    
    func testNormalizeWithNegativePosition() {
        let position = Position(x: -1, y: -1)
        let normalizedPosition = tinyMap.normalize(position: position)
        XCTAssertEqual(normalizedPosition, Position(x: 4, y: 4))
        
        // Test that normalization is stable
        XCTAssertEqual(tinyMap.normalize(position: normalizedPosition), normalizedPosition)
    }
    
    func testNormalizeWithLargePosition() {
        let position = Position(x: 6, y: 6)
        let normalizedPosition = tinyMap.normalize(position: position)
        XCTAssertEqual(normalizedPosition, Position(x: 1, y: 1))
        
        // Test that normalization is stable
        XCTAssertEqual(tinyMap.normalize(position: normalizedPosition), normalizedPosition)
    }
}

class MapCellTests: XCTestCase {
    let initialPosition = Position(x: 10, y: 10)
    let playerId = "1"
    let dropoffId = "2"
    let shipyardId = "3"
    let shipId = "4"
    
    func testEmptyCell() {
        let cell = MapCell(position: initialPosition, haliteAmount: 0, ship: nil, structure: nil)
        
        XCTAssertTrue(cell.isEmpty)
        XCTAssertFalse(cell.isOccupied)
        XCTAssertFalse(cell.hasStructure)
        XCTAssertNil(cell.structureType)
    }
    
    func testCellWithShip() {
        let ship = Ship(owner: playerId, id: shipId, position: initialPosition, haliteAmount: 0)
        let cell = MapCell(position: initialPosition, haliteAmount: 0, ship: ship, structure: nil)
        
        XCTAssertFalse(cell.isEmpty)
        XCTAssertTrue(cell.isOccupied)
        XCTAssertFalse(cell.hasStructure)
        XCTAssertNil(cell.structureType)
    }
    
    func testCellWithDropoff() {
        let structure = Dropoff(owner: playerId, id: dropoffId, position: initialPosition)
        let cell = MapCell(position: initialPosition, haliteAmount: 0, ship: nil, structure: structure)
        
        XCTAssertFalse(cell.isEmpty)
        XCTAssertFalse(cell.isOccupied)
        XCTAssertTrue(cell.hasStructure)
        XCTAssertEqual(cell.structureType, .dropoff)
    }
    
    func testCellWithShipyard() {
        let structure = Shipyard(owner: playerId, id: shipyardId, position: initialPosition)
        let cell = MapCell(position: initialPosition, haliteAmount: 0, ship: nil, structure: structure)
        
        XCTAssertFalse(cell.isEmpty)
        XCTAssertFalse(cell.isOccupied)
        XCTAssertTrue(cell.hasStructure)
        XCTAssertEqual(cell.structureType, .shipyard)
    }
    
    func testCellWithShipAndDropoff() {
        let ship = Ship(owner: playerId, id: shipId, position: initialPosition, haliteAmount: 0)
        let structure = Shipyard(owner: playerId, id: shipyardId, position: initialPosition)
        let cell = MapCell(position: initialPosition, haliteAmount: 0, ship: ship, structure: structure)
        
        XCTAssertFalse(cell.isEmpty)
        XCTAssertTrue(cell.isOccupied)
        XCTAssertTrue(cell.hasStructure)
        XCTAssertEqual(cell.structureType, .shipyard)
    }
}
