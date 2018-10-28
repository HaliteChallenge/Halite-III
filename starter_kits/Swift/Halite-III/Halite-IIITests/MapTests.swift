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
    let playerId = 1
    let shipId = 2
    
    func testSubscript() {
        let middle = Position(x: 2, y: 2)
        
        let cell = tinyMap[middle]
        XCTAssert(cell.isEmpty)
    }
    
    // MARK - Test Initialization
    func testInitializationWithNoHalite() {
        let size = 3
        let map = Map(width: size, height: size)
        
        (0..<size).forEach { row in
            (0..<size).forEach{ column in
                XCTAssertEqual(map[Position(x: column, y: row)].haliteAmount, 0)
            }
        }
    }
    
    func testInitializationWithSomeHalite() {
        let size = 3
        let amount = 10
        let halite = [
            [amount, 0, 0],
            [0, amount, 0],
            [0, 0, amount]
        ]
        
        let map = Map(width: size, height: size, initialHalite: halite)
        
        (0..<size).forEach { row in
            (0..<size).forEach{ column in
                let position = Position(x: column, y: row)
                if row == column {
                    XCTAssertEqual(map[position].haliteAmount, amount)
                } else {
                    XCTAssertEqual(map[position].haliteAmount, 0)
                }
            }
        }
    }
    
    // MARK: - Test normalize method
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
    
    // MARK: - Test distance method
    func testHorizontalDistance() {
        let from = Position(x: 1, y: 1)
        let to = Position(x: 3, y: 1)
        let distance = tinyMap.calculateDistance(source: from, target: to)
        
        XCTAssertEqual(distance, 2)
    }
    
    func testVerticalDistance() {
        let from = Position(x: 1, y: 1)
        let to = Position(x: 1, y: 3)
        let distance = tinyMap.calculateDistance(source: from, target: to)
        
        XCTAssertEqual(distance, 2)
    }
    
    func testDiagonalDistance() {
        let from = Position(x: 1, y: 1)
        let to = Position(x: 3, y: 3)
        let distance = tinyMap.calculateDistance(source: from, target: to)
        
        XCTAssertEqual(distance, 4)
    }
    
    func testWraparoundDistance() {
        // This is interesting, because the shortest distance is wrapping around the map.
        let bottomRight = Position(x: 4, y: 4)
        let topLeft = Position(x: 0, y: 0)
        let distance = tinyMap.calculateDistance(source: bottomRight, target: topLeft)
        
        XCTAssertEqual(distance, 2)
    }
    
    func testDistanceWithAbnormalValues() {
        // The wraparound is also faster here. Without wraparound, the distance is 6
        let tooBig = Position(x: 5, y: 5)
        let tooSmall = Position(x: -2, y: -2)
        let distance = tinyMap.calculateDistance(source: tooBig, target: tooSmall)
        
        XCTAssertEqual(distance, 4)
    }
    
    // MARK: Convenience methods
    func testDistance() {
        XCTAssertEqual(tinyMap.distance(from: Position(x: 1, y: 1), to: Position(x: 3, y: 3)), 4)
        XCTAssertEqual(tinyMap.distance(from: Position(x: 4, y: 4), to: Position(x: 0, y: 0)), 2)
        XCTAssertEqual(tinyMap.distance(from: Position(x: 5, y: 5), to: Position(x: -2, y: -2)), 4)
    }
    
    // MARK: - Test getUnsafeMoves
    func testUnsafeMovesWithSameSourceAndDestination() {
        let point = Position(x: 2, y: 2)
        let unsafeMoves = tinyMap.getUnsafeMoves(source: point, destination: point)
        
        XCTAssertTrue(unsafeMoves.isEmpty)
    }
    
    func testUnsafeMovesWithEqualOptions() {
        let topLeft = Position(x: 1, y: 1)
        let bottomRight = Position(x: 2, y: 2)
        let unsafeMoves = tinyMap.getUnsafeMoves(source: topLeft, destination: bottomRight)
        
        XCTAssertEqual(unsafeMoves.count, 2)
        XCTAssert(unsafeMoves.contains(.east))
        XCTAssert(unsafeMoves.contains(.south))
        
        let backwardsMoves = tinyMap.getUnsafeMoves(source: bottomRight, destination: topLeft)
        
        XCTAssertEqual(backwardsMoves.count, 2)
        XCTAssert(backwardsMoves.contains(.north))
        XCTAssert(backwardsMoves.contains(.west))
    }
    
    func testUnsafeMovesWithHorizontalOptions() {
        let left = Position(x: 1, y: 1)
        let right = Position(x: 2, y: 1)
        let unsafeMoves = tinyMap.getUnsafeMoves(source: left, destination: right)
        
        XCTAssertEqual(unsafeMoves.count, 1)
        XCTAssert(unsafeMoves.contains(.east))
        
        let backwardsMoves = tinyMap.getUnsafeMoves(source: right, destination: left)
        
        XCTAssertEqual(backwardsMoves.count, 1)
        XCTAssert(backwardsMoves.contains(.west))
    }
    
    func testUnsafeMovesWithVerticalOptions() {
        let top = Position(x: 1, y: 1)
        let bottom = Position(x: 1, y: 2)
        let unsafeMoves = tinyMap.getUnsafeMoves(source: top, destination: bottom)
        
        XCTAssertEqual(unsafeMoves.count, 1)
        XCTAssert(unsafeMoves.contains(.south))
        
        let backwardsMoves = tinyMap.getUnsafeMoves(source: bottom, destination: top)
        
        XCTAssertEqual(backwardsMoves.count, 1)
        XCTAssert(backwardsMoves.contains(.north))
    }
    
    func testUnsafeMovesWithLargeHorizontalDistance() {
        let left = Position(x: 1, y: 1)
        let right = Position(x: 3, y: 1)
        let unsafeMoves = tinyMap.getUnsafeMoves(source: left, destination: right)
        
        XCTAssertEqual(unsafeMoves.count, 1)
        XCTAssert(unsafeMoves.contains(.east))
    }
    
    func testUnsafeMovesWithDiagonalWraparound() {
        let minExtreme = Position(x: 0, y: 0)
        let maxExtreme = Position(x: tinyMap.width - 1, y: tinyMap.height - 1)
        let unsafeMoves = tinyMap.getUnsafeMoves(source: minExtreme, destination: maxExtreme)
        
        XCTAssertEqual(unsafeMoves.count, 2)
        XCTAssert(unsafeMoves.contains(.north))
        XCTAssert(unsafeMoves.contains(.west))
        
        let backwardsMoves = tinyMap.getUnsafeMoves(source: maxExtreme, destination: minExtreme)
        
        XCTAssertEqual(backwardsMoves.count, 2)
        XCTAssert(backwardsMoves.contains(.south))
        XCTAssert(backwardsMoves.contains(.east))
    }
    
    // MARK: - Test naiveNavigate
    func testNaiveNavigateWithShipAtDestination() {
        let shipPosition = Position(x: 1, y: 1)
        let ship = Ship(owner: playerId, id: shipId, position: shipPosition, haliteAmount: 0)
        
        let direction = tinyMap.naiveNavigate(ship: ship, destination: shipPosition)
        XCTAssertEqual(direction, .still)
    }
    
    func testNaiveNavigateEast() {
        let shipPosition = Position(x: 1, y: 1)
        let shipDestination = Position(x: 2, y: 1)
        let ship = Ship(owner: playerId, id: shipId, position: shipPosition, haliteAmount: 0)
        
        let direction = tinyMap.naiveNavigate(ship: ship, destination: shipDestination)
        XCTAssertEqual(direction, .east)
    }
    
    func testNaiveNavigateWest() {
        let shipPosition = Position(x: 1, y: 1)
        let shipDestination = Position(x: 0, y: 1)
        let ship = Ship(owner: playerId, id: shipId, position: shipPosition, haliteAmount: 0)
        
        let direction = tinyMap.naiveNavigate(ship: ship, destination: shipDestination)
        XCTAssertEqual(direction, .west)
    }
    
    func testNaiveNavigateNorth() {
        let shipPosition = Position(x: 1, y: 1)
        let shipDestination = Position(x: 1, y: 0)
        let ship = Ship(owner: playerId, id: shipId, position: shipPosition, haliteAmount: 0)
        
        let direction = tinyMap.naiveNavigate(ship: ship, destination: shipDestination)
        XCTAssertEqual(direction, .north)
    }
    
    func testNaiveNavigateSouth() {
        let shipPosition = Position(x: 1, y: 1)
        let shipDestination = Position(x: 1, y: 2)
        let ship = Ship(owner: playerId, id: shipId, position: shipPosition, haliteAmount: 0)
        
        let direction = tinyMap.naiveNavigate(ship: ship, destination: shipDestination)
        XCTAssertEqual(direction, .south)
    }
    
    func testNaiveNavigateNorthWithWraparound() {
        let shipPosition = Position(x: 1, y: 1)
        let shipDestination = Position(x: 1, y: 4)
        let ship = Ship(owner: playerId, id: shipId, position: shipPosition, haliteAmount: 0)
        
        let direction = tinyMap.naiveNavigate(ship: ship, destination: shipDestination)
        XCTAssertEqual(direction, .north)
    }
    
    func testNaiveNavigateWestWithWraparound() {
        let shipPosition = Position(x: 1, y: 1)
        let shipDestination = Position(x: 4, y: 1)
        let ship = Ship(owner: playerId, id: shipId, position: shipPosition, haliteAmount: 0)
        
        let direction = tinyMap.naiveNavigate(ship: ship, destination: shipDestination)
        XCTAssertEqual(direction, .west)
    }
    
    func testNaiveNavigateSouthEast() {
        let shipPosition = Position(x: 1, y: 1)
        let shipDestination = Position(x: 2, y: 2)
        let ship = Ship(owner: playerId, id: shipId, position: shipPosition, haliteAmount: 0)
        
        let direction = tinyMap.naiveNavigate(ship: ship, destination: shipDestination)
        XCTAssert(direction == .south || direction == .east)
    }
    
    func testNaiveNavigateSouthEastWithWraparound() {
        let shipPosition = Position(x: 4, y: 4)
        let shipDestination = Position(x: 0, y: 0)
        let ship = Ship(owner: playerId, id: shipId, position: shipPosition, haliteAmount: 0)
        
        let direction = tinyMap.naiveNavigate(ship: ship, destination: shipDestination)
        XCTAssert(direction == .south || direction == .east)
    }
}

// MARK: - Map.Cell Tests
class MapCellTests: XCTestCase {
    // MARK: Common properties
    let initialPosition = Position(x: 10, y: 10)
    let playerId = 1
    let dropoffId = 2
    let shipyardId = 3
    let shipId = 4
    
    // MARK: - Test Cell property methods
    func testEmptyCell() {
        let cell = Map.Cell(position: initialPosition, haliteAmount: 0, ship: nil, structure: nil)
        
        XCTAssertTrue(cell.isEmpty)
        XCTAssertFalse(cell.isOccupied)
        XCTAssertFalse(cell.hasStructure)
        XCTAssertNil(cell.structureType)
    }
    
    func testCellWithShip() {
        let ship = Ship(owner: playerId, id: shipId, position: initialPosition, haliteAmount: 0)
        let cell = Map.Cell(position: initialPosition, haliteAmount: 0, ship: ship, structure: nil)
        
        XCTAssertFalse(cell.isEmpty)
        XCTAssertTrue(cell.isOccupied)
        XCTAssertFalse(cell.hasStructure)
        XCTAssertNil(cell.structureType)
    }
    
    func testCellWithDropoff() {
        let structure = Dropoff(owner: playerId, id: dropoffId, position: initialPosition)
        let cell = Map.Cell(position: initialPosition, haliteAmount: 0, ship: nil, structure: structure)
        
        XCTAssertFalse(cell.isEmpty)
        XCTAssertFalse(cell.isOccupied)
        XCTAssertTrue(cell.hasStructure)
        XCTAssertEqual(cell.structureType, .dropoff)
    }
    
    func testCellWithShipyard() {
        let structure = Shipyard(owner: playerId, id: shipyardId, position: initialPosition)
        let cell = Map.Cell(position: initialPosition, haliteAmount: 0, ship: nil, structure: structure)
        
        XCTAssertFalse(cell.isEmpty)
        XCTAssertFalse(cell.isOccupied)
        XCTAssertTrue(cell.hasStructure)
        XCTAssertEqual(cell.structureType, .shipyard)
    }
    
    func testCellWithShipAndDropoff() {
        let ship = Ship(owner: playerId, id: shipId, position: initialPosition, haliteAmount: 0)
        let structure = Shipyard(owner: playerId, id: shipyardId, position: initialPosition)
        let cell = Map.Cell(position: initialPosition, haliteAmount: 0, ship: ship, structure: structure)
        
        XCTAssertFalse(cell.isEmpty)
        XCTAssertTrue(cell.isOccupied)
        XCTAssertTrue(cell.hasStructure)
        XCTAssertEqual(cell.structureType, .shipyard)
    }
    
    // MARK: - Test mark unsafe
    func testMarkUnsafe() {
        let ship = Ship(owner: playerId, id: shipId, position: initialPosition, haliteAmount: 0)
        let cell = Map.Cell(position: initialPosition, haliteAmount: 0, ship: nil, structure: nil)
        
        XCTAssertFalse(cell.isOccupied)
        cell.markUnsafe(ship: ship)
        XCTAssertTrue(cell.isOccupied)
    }

}
