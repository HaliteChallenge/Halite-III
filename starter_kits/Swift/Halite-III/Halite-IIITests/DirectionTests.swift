//
//  DirectionTests.swift
//  Halite-IIITests
//
//  Created by Chris Downie on 10/25/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import XCTest

class DirectionTests: XCTestCase {
    func testInversion() {
        XCTAssertEqual(Direction.south, Direction.invert(.north))
        XCTAssertEqual(Direction.north, Direction.invert(.south))
        XCTAssertEqual(Direction.west, Direction.invert(.east))
        XCTAssertEqual(Direction.east, Direction.invert(.west))
        XCTAssertEqual(Direction.still, Direction.invert(.still))
    }
    
    func testUnaryMinus() {
        XCTAssertEqual(Direction.south, -Direction.north)
        XCTAssertEqual(Direction.north, -Direction.south)
        XCTAssertEqual(Direction.west, -Direction.east)
        XCTAssertEqual(Direction.east, -Direction.west)
        XCTAssertEqual(Direction.still, -Direction.still)
    }
}
