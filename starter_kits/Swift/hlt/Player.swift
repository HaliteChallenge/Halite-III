import Foundation

struct Position {
    let x: Int
    let y: Int
}

struct Player {
    let id: String
    let shipyard: Shipyard
    let haliteAmount: Int
    
    // MARK: Ships
    /// Returns the ship object associated with the ship id provided as an argument.
    ///
    /// - Parameter ship: The ship ID to query for.
    /// - Returns: The ship object associated with the ship ID provided as an argument.
    func get(ship: Ship.ID) -> Ship? {
        return nil
    }
    
    /// Returns a list of all ship objects.
    ///
    /// - Returns: All ship objects.
    func getShips() -> [Ship] {
        return []
    }
    
    /// Checks if you have a ship with this id.
    ///
    /// - Parameter ship: The ship ID to query for.
    /// - Returns: True if you have a ship with this id.
    func has(ship: Ship.ID) -> Bool {
        return false
    }
    
    // MARK: Dropoffs
    /// Returns the dropoff object associated with the dropoff id provided as an argument.
    ///
    /// - Parameter dropoff: The dropoff ID to query for
    /// - Returns: The dropoff object associated with the dropoff id provided as an argument.
    func get(dropoff: Dropoff.ID) -> Dropoff? {
        return nil
    }

    /// Returns a list of all dropoff objects.
    ///
    /// - Returns: All dropoff objects.
    func getDropoffs() -> [Dropoff] {
        return []
    }
}

struct Ship {
    typealias ID = String
    
    let owner: Int // what is this?
    let id: ID
    let position: Position
    let haliteAmount: Int
    
    /// Cargo: returns a boolean True if ship is carrying 1,000 halite (the maximum). Otherwise returns False.
    ///
    /// - Returns: True if ship is carrying 1,000 halite (the maximum). Otherwise returns False.
    func isFull() -> Bool {
        return haliteAmount >= Constant.shared.MAX_ENERGY
    }
    
    /// Returns an engine command to convert this ship into a dropoff.
    ///
    /// Ships can be converted into dropoff sites at their present location. The conversion costs 4,000 halite,
    /// deducted from total current stored halite. The converted ship’s halite cargo and the halite in the sea under
    /// the new dropoff is credited to the player. These credits resolve first, and can be used toward the cost of
    /// the dropoff.
    ///
    /// - Returns: An engine command to convert this ship into a dropoff.
    func makeDropoff() -> Command {
        return .construct
    }
    
    /// Returns an engine command to keep this ship where it is and collect halite.
    ///
    /// Ships can collect 25% of the halite from the sea at their present location, rounded up to the nearest integer.
    ///
    /// - Returns: An engine command to keep this ship where it is and collect halite.
    func stayStill() -> Command {
        return .move(direction: .north)
    }
    
    /// Returns an engine command to move this ship in a direction without checking for collisions.
    ///
    /// Ships can move one square in a cardinal direction per turn. Each move costs 10% of the halite available in
    /// the sea at the ship’s starting location, debited from the ships’ cargo. The direction of the move is
    /// communicated via the command queue.
    ///
    /// - Parameter direction: The direction to move
    /// - Returns: An engine command to move this ship in a direction without checking for collisions.
    func move(direction: Direction) -> Command {
        return .move(direction: direction)
    }
}

struct Shipyard {
    typealias ID = String
    
    let owner: String
    let id: ID
    let position: Position
    
    /// Returns an engine command to generate a new ship.
    ///
    /// - Returns: An engine command to generate a new ship.
    func spawn() -> Command {
        return .generate
    }
}

/// You create a dropoff at any location on the map by converting a ship. Ships can store halite at a dropoff point
/// just as they would at the shipyard. If two dropoffs are constructed in the same location, the engine returns an
/// error and the construct command fails. The player class has the methods to access dropoffs.
struct Dropoff {
    typealias ID = String
    
    let owner: String
    let id: ID
    let position: Position
}
