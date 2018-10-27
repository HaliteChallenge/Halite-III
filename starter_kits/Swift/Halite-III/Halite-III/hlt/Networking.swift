//
//  Networking.swift
//  Halite-III
//
//  Created by Chris Downie on 10/27/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//

import Foundation

class Networking {
    // MARK: - Typed input responses
    struct ConstantsInput {
        let json: String
    }
    
    struct IdentityInput {
        let numPlayers: Int
        let myId: Player.ID
    }
    
    struct PlayerInput {
        let playerId: Player.ID
        let shipyardPosition: Position
    }
    
    struct MapInput {
        let mapWidth: Int
        let mapHeight: Int
    }
    
    struct MapRowInput {
        let haliteAmount: [Int]
    }
    
    struct MapUpdates {
        struct Update {
            let position: Position
            let haliteAmount: Int
        }
        
        let updates: [Update]
    }
    
    // MARK - Read methods
    func readConstants() -> ConstantsInput {
        let line = readLine(message: "Failed to read constant json input.")
        return ConstantsInput(json: line)
    }
    
    func readIdentity() -> IdentityInput {
        let line = readLine(message: "Failed to read player identity input.")
        let numbers = line.split(separator: " ").compactMap { Int($0) }
        assert(numbers.count == 2)
        return IdentityInput(numPlayers: numbers[0], myId: numbers[1])
    }
    
    func readPlayer() -> PlayerInput {
        let line = readLine(message: "Failed to read player's shipyard location.")
        let numbers = line.split(separator: " ").compactMap { Int($0) }
        assert(numbers.count == 3)
        let position = Position(x: numbers[1], y: numbers[2])
        return PlayerInput(playerId: numbers[0], shipyardPosition: position)
    }
    
    func readMapDimensions() -> MapInput {
        let line = readLine(message: "Failed to read map dimensions.")
        let numbers = line.split(separator: " ").compactMap { Int($0) }
        assert(numbers.count == 2)
        return MapInput(mapWidth: numbers[0], mapHeight: numbers[1])
    }
    
    func readMapRow() -> MapRowInput {
        let line = readLine(message: "Failed to read map row's initial halite amount.")
        let numbers = line.split(separator: " ").compactMap { Int($0) }
        return MapRowInput(haliteAmount: numbers)
    }
    
    func readMapUpdates() -> MapUpdates {
        let linesToReadString = readLine(message: "Failed to read how many updates we need to process")
        guard let linesToRead = Int(linesToReadString) else {
            fatalError("The number of updates to expect wasn't a number.")
        }
        let range = 0..<linesToRead
        let updates = range.map { updateNumber -> Networking.MapUpdates.Update in
            let line = readLine(message: "Failed to read a map's update data for update #\(updateNumber).")
            let numbers = line.split(separator: " ").compactMap { Int($0) }
            assert(numbers.count == 3)
            let position = Position(x: numbers[0], y: numbers[1])
            return MapUpdates.Update(position: position, haliteAmount: numbers[2])
        }
        
        return MapUpdates(updates: updates)
    }
    
    // MARK: - Private helpers
    private func readLine(message: String) -> String {
        guard let input = Swift.readLine(strippingNewline: true) else {
            fatalError(message)
        }
        return input
    }
}
