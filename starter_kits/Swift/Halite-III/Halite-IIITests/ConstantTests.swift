//
//  ConstantTests.swift
//  Halite-IIITests
//
//  Created by Chris Downie on 10/26/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import XCTest

class ConstantTests: XCTestCase {
    func testAllInputs() {
        // This is all the input I captured from the initial launch of the halite program
        let allInput = """
        {
          "CAPTURE_ENABLED": false,
          "CAPTURE_RADIUS": 3,
          "DEFAULT_MAP_HEIGHT": 32,
          "DEFAULT_MAP_WIDTH": 32,
          "DROPOFF_COST": 4000,
          "DROPOFF_PENALTY_RATIO": 4,
          "EXTRACT_RATIO": 4,
          "FACTOR_EXP_1": 2.0,
          "FACTOR_EXP_2": 2.0,
          "INITIAL_ENERGY": 5000,
          "INSPIRATION_ENABLED": true,
          "INSPIRATION_RADIUS": 4,
          "INSPIRATION_SHIP_COUNT": 2,
          "INSPIRED_BONUS_MULTIPLIER": 2.0,
          "INSPIRED_EXTRACT_RATIO": 4,
          "INSPIRED_MOVE_COST_RATIO": 10,
          "MAX_CELL_PRODUCTION": 1000,
          "MAX_ENERGY": 1000,
          "MAX_PLAYERS": 16,
          "MAX_TURNS": 400,
          "MAX_TURN_THRESHOLD": 64,
          "MIN_CELL_PRODUCTION": 900,
          "MIN_TURNS": 400,
          "MIN_TURN_THRESHOLD": 32,
          "MOVE_COST_RATIO": 10,
          "NEW_ENTITY_ENERGY_COST": 1000,
          "PERSISTENCE": 0.7,
          "SHIPS_ABOVE_FOR_CAPTURE": 3,
          "STRICT_ERRORS": false,
          "game_seed": 1540462870
        }
        """
        
        let constants = try? Constant.from(json: allInput)
        XCTAssertNotNil(constants)
    }

    func testMinimumInput() {
        // This is just all the globals declared in the python implementation
        let minInput = """
        {
          "NEW_ENTITY_ENERGY_COST": 1000,
          "DROPOFF_COST": 4000,
          "MAX_ENERGY": 1000,
          "MAX_TURNS": 400,
          "EXTRACT_RATIO": 4,
          "MOVE_COST_RATIO": 10,
          "INSPIRATION_ENABLED": true,
          "INSPIRATION_RADIUS": 4,
          "INSPIRATION_SHIP_COUNT": 2,
          "INSPIRED_BONUS_MULTIPLIER": 2.0,
          "INSPIRED_EXTRACT_RATIO": 4,
          "INSPIRED_MOVE_COST_RATIO": 10,
        }
        """
        
        let constants = try? Constant.from(json: minInput)
        XCTAssertNotNil(constants)
    }
}
