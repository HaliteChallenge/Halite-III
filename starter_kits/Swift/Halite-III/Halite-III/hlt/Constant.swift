//
//  Constant.swift
//  Halite-III
//
//  Created by Chris Downie on 10/24/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//
import Foundation

struct Constant: Codable {
    // These match all the constants we see in the input JSON exactly.
    let CAPTURE_ENABLED: Bool
    let CAPTURE_RADIUS: Int
    let DEFAULT_MAP_HEIGHT: Int
    let DEFAULT_MAP_WIDTH: Int
    let DROPOFF_COST: Int
    let DROPOFF_PENALTY_RATIO: Int
    let EXTRACT_RATIO: Int
    let FACTOR_EXP_1: Double
    let FACTOR_EXP_2: Double
    let INITIAL_ENERGY: Int
    let INSPIRATION_ENABLED: Bool
    let INSPIRATION_SHIP_COUNT: Int
    let INSPIRED_BONUS_MULTIPLIER: Double
    let INSPIRED_EXTRACT_RATIO: Int
    let INSPIRED_MOVE_COST_RATIO: Int
    let MAX_CELL_PRODUCTION: Int
    let MAX_ENERGY: Int
    let MAX_PLAYERS: Int
    let MAX_TURNS: Int
    let MAX_TURN_THRESHOLD: Int
    let MIN_CELL_PRODUCTION: Int
    let MIN_TURNS: Int
    let MIN_TURN_THRESHOLD: Int
    let MOVE_COST_RATIO: Int
    let NEW_ENTITY_ENERGY_COST: Int
    let PERSISTENCE: Double
    let SHIPS_ABOVE_FOR_CAPTURE: Int
    let STRICT_ERRORS: Bool
    let game_seed: Int
    
    // Here are some convenience accessors with friendlier names I noticed in the code
    var MAX_HALITE: Int {
        return MAX_ENERGY
    }
    var SHIP_COST: Int {
        return NEW_ENTITY_ENERGY_COST
    }
    
    static var shared: Constant = {
        let log = Log(filename: "output.log")
        
        let input = readLine(strippingNewline: false)!
        log.debug(input)
        let data = input.data(using: .utf8)!
        do {
            let constants = try JSONDecoder().decode(Constant.self, from: data)
            log.debug("got constants.")
            return constants
        } catch {
            log.error("\(error)")
            fatalError("Failed to load constants.")
        }
    }()
}
