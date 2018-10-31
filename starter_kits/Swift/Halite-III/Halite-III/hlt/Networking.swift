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
    
    struct MapUpdate {
        let position: Position
        let haliteAmount: Int
    }
    
    struct PlayerUpdate {
        let playerID: Player.ID
        let halite: Int
        
        let ships: [Ship]
        let dropoffs: [Dropoff]
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
    
    func readMapUpdates() -> [MapUpdate] {
        let linesToReadString = readLine(message: "Failed to read how many updates we need to process")
        guard let linesToRead = Int(linesToReadString) else {
            fatalError("The number of updates to expect wasn't a number.")
        }
        let range = 0..<linesToRead
        let updates = range.map { updateNumber -> Networking.MapUpdate in
            let line = readLine(message: "Failed to read a map's update data for update #\(updateNumber).")
            let numbers = line.split(separator: " ").compactMap { Int($0) }
            assert(numbers.count == 3)
            let position = Position(x: numbers[0], y: numbers[1])
            return MapUpdate(position: position, haliteAmount: numbers[2])
        }
        
        return updates
    }
    
    func readTurnNumber() -> Int {
        let line = readLine(message: "Failed to read the new turn number.")
        guard let turnNumber = Int(line) else {
            fatalError("The number of updates to expect wasn't a number.")
        }
        return turnNumber
    }
    
    func readPlayerUpdate() -> PlayerUpdate {
        let line = readLine(message: "Failed to read updated player information.")
        let numbers = line.split(separator: " ").compactMap { Int($0) }
        assert(numbers.count == 4)
        let playerId = numbers[0]
        let shipCount = numbers[1]
        let dropoffCount = numbers[2]
        let playerHalite = numbers[3]
        
        let ships = (0..<shipCount).map { _ -> Ship in
            let line = readLine(message: "Failed to read a ship from a player update")
            let numbers = line.split(separator: " ").compactMap { Int($0) }
            assert(numbers.count == 4)
            return Ship(owner: playerId,
                        id: numbers[0],
                        position: Position(x: numbers[1], y: numbers[2]),
                        haliteAmount: numbers[3])
        }
        
        let dropoffs = (0..<dropoffCount).map { _ -> Dropoff in
            let line = readLine(message: "Failed to read a dropoff from a player update")
            let numbers = line.split(separator: " ").compactMap { Int($0) }
            assert(numbers.count == 3)
            return Dropoff(owner: playerId,
                           id: numbers[0],
                           position: Position(x: numbers[1], y: numbers[2]))
        }
        
        return PlayerUpdate(playerID: playerId, halite: playerHalite, ships: ships, dropoffs: dropoffs)
    }
    
    func write(commands: [Command]) {
        let output = commands.map { string(from: $0) }.joined(separator: " ")
        write(string: output)
    }
    
    func writeReady(botName: String) {
        write(string: botName)
    }
    
    private func write(string: String) {
        print(string)
        fflush(stdout)
    }
    
    // MARK: - Private helpers
    private func readLine(message: String) -> String {
        guard let input = Swift.readLine() else {
            fatalError(message)
        }
        return input
    }
    
    private func string(from command: Command) -> String {
        switch command {
        case .move(let shipId, let direction):
            return "m \(shipId) \(string(from: direction))"
        case .generate:
            return "g"
        case .construct(let shipId):
            return "c \(shipId)"
        }
    }
    
    private func string(from direction: Direction) -> String {
        switch direction {
        case .north:
            return "n"
        case .south:
            return "s"
        case .east:
            return "e"
        case .west:
            return "w"
        case .still:
            return "s"
        }
    }
}
