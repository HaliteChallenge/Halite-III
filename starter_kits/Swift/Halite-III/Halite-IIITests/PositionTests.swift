//
//  PositionTests.swift
//  Halite-IIITests
//
//  Created by Chris Downie on 10/25/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import XCTest

class PositionTests: XCTestCase {
    func testNorthOffset() {
        let position = Position(x: 10, y: 10)
        let newPosition = position.directionalOffset(.north)
        XCTAssertEqual(newPosition.x, 10)
        XCTAssertEqual(newPosition.y, 9)
    }
}
