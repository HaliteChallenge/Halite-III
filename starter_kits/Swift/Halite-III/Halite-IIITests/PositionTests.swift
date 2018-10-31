//
//  PositionTests.swift
//  Halite-IIITests
//
//  Created by Chris Downie on 10/25/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import XCTest

class PositionTests: XCTestCase {
    let initialPosition = Position(x: 10, y: 10)
    let southPosition = Position(x: 10, y: 11)
    let northPosition = Position(x: 10, y: 9)
    let westPosition = Position(x: 9, y: 10)
    let eastPosition = Position(x: 11, y: 10)
    
    // MARK: - Testing directionalOffset
    func testNorthOffset() {
        let newPosition = initialPosition.directionalOffset(.north)
        XCTAssertEqual(newPosition, northPosition)
    }
    
    func testSouthOffset() {
        let newPosition = initialPosition.directionalOffset(.south)
        XCTAssertEqual(newPosition, southPosition)
    }
    
    func testWestOffset() {
        let newPosition = initialPosition.directionalOffset(.west)
        XCTAssertEqual(newPosition, westPosition)
    }
    
    func testEastOffset() {
        let newPosition = initialPosition.directionalOffset(.east)
        XCTAssertEqual(newPosition, eastPosition)
    }
    
    func testNoOffset() {
        let newPosition = initialPosition.directionalOffset(.still)
        XCTAssertEqual(newPosition, initialPosition)
    }
    
    // MARK: Testing getSurroundingCardinals
    func testGetSurroundingCardinals() {
        let cardinals = initialPosition.getSurroundingCardinals()
        
        XCTAssertEqual(cardinals.count, 4)
        XCTAssert(cardinals.contains(northPosition))
        XCTAssert(cardinals.contains(southPosition))
        XCTAssert(cardinals.contains(eastPosition))
        XCTAssert(cardinals.contains(westPosition))
    }
    
    func testSurroundingCardinals() {
        // This is the same as getSurroundingCardinals, but is more familiar to Swift
        let cardinals = initialPosition.surroundingCardinals
        
        XCTAssertEqual(cardinals.count, 4)
        XCTAssert(cardinals.contains(northPosition))
        XCTAssert(cardinals.contains(southPosition))
        XCTAssert(cardinals.contains(eastPosition))
        XCTAssert(cardinals.contains(westPosition))
    }
    
    // MARK: Test addition & subtraction
    // This is mostly tested by .directionalOffset(:). So this is more documentation than test coverage.
    func testPositionAddition() {
        let newPosition = initialPosition + .north
        XCTAssertEqual(newPosition, northPosition)
    }
    
    func testPositionSubtraction() {
        let newPosition = initialPosition - .north
        XCTAssertEqual(newPosition, southPosition)
    }
    
    func testInPlaceAddition() {
        var newPosition = initialPosition
        newPosition += .north
        XCTAssertEqual(newPosition, northPosition)
    }
    
    func testInPlaceSubtraction() {
        var newPosition = initialPosition
        newPosition -= .north
        XCTAssertEqual(newPosition, southPosition)
    }
    
    func testNonNormalizedResult() {
        let newPosition = Position(x: 0, y: 0) + .north
        XCTAssertEqual(newPosition, Position(x: 0, y: -1))
    }
}
