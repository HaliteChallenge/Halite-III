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
        XCTAssert(cell.isEmpty())
    }
}
