//
//  Constant.swift
//  Halite-III
//
//  Created by Chris Downie on 10/24/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//
import Foundation

struct Constant: Codable {
    // MARK: - Constants documented in the python implementation
    
    /// The cost to build a dropoff.
    let DROPOFF_COST: Int
    /// 1/EXTRACT_RATIO halite (truncated) is collected from a square per turn.
    let EXTRACT_RATIO: Int
    /// Whether inspiration is enabled.
    let INSPIRATION_ENABLED: Bool
    /// A ship is inspired if at least INSPIRATION_SHIP_COUNT opponent ships are within this Manhattan distance.
    let INSPIRATION_RADIUS: Int
    /// A ship is inspired if at least this many opponent ships are within INSPIRATION_RADIUS distance.
    let INSPIRATION_SHIP_COUNT: Int
    /// An inspired ship that removes Y halite from a cell collects X*Y additional halite.
    let INSPIRED_BONUS_MULTIPLIER: Double
    /// An inspired ship mines 1/X halite from a cell per turn instead.
    let INSPIRED_EXTRACT_RATIO: Int
    /// An inspired ship instead spends 1/X% halite to move.
    let INSPIRED_MOVE_COST_RATIO: Int
    /// The maximum amount of halite a ship can carry.
    let MAX_ENERGY: Int
    /// The maximum number of turns a game can last. This reflects the fact that smaller maps play for fewer turns.
    let MAX_TURNS: Int
    /// 1/MOVE_COST_RATIO halite (truncated) is needed to move off a cell.
    let MOVE_COST_RATIO: Int
    /// The cost to build a single ship.
    let NEW_ENTITY_ENERGY_COST: Int
    
    /// MARK: Here are some convenience accessors with friendlier names I noticed in the code
    /// The maximum amount of halite a ship can carry.
    var MAX_HALITE: Int {
        return MAX_ENERGY
    }
    /// The cost to build a single ship.
    var SHIP_COST: Int {
        return NEW_ENTITY_ENERGY_COST
    }
    
    // MARK: - Other constants observed, but not required. Use with caution.
    let CAPTURE_ENABLED: Bool?
    let CAPTURE_RADIUS: Int?
    let DEFAULT_MAP_HEIGHT: Int?
    let DEFAULT_MAP_WIDTH: Int?
    let DROPOFF_PENALTY_RATIO: Int?
    let FACTOR_EXP_1: Double?
    let FACTOR_EXP_2: Double?
    let INITIAL_ENERGY: Int?
    let MAX_CELL_PRODUCTION: Int?
    let MAX_PLAYERS: Int?
    let MAX_TURN_THRESHOLD: Int?
    let MIN_CELL_PRODUCTION: Int?
    let MIN_TURNS: Int?
    let MIN_TURN_THRESHOLD: Int?
    let PERSISTENCE: Double?
    let SHIPS_ABOVE_FOR_CAPTURE: Int?
    let STRICT_ERRORS: Bool?
    let game_seed: Int?
    
    static private var _shared: Constant?
    static var shared: Constant {
        guard let _shared = _shared else {
            fatalError("You need to call loadShared before accessing this singleton.")
        }
        return _shared
    }
    
    static func from(json: String) throws -> Constant {
        guard let data = json.data(using: .utf8) else {
            throw ConstantError.jsonIsNotUTF8
        }
        let constants = try JSONDecoder().decode(Constant.self, from: data)
        return constants
    }
    
    static func loadShared(with json: String) throws {
        _shared = try from(json: json)
    }
}

enum ConstantError: Error {
    case jsonIsNotUTF8
}
