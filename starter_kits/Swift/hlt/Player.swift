import Foundation

struct Position {
    let x: Int
    let y: Int
    
    /// Returns a new position based on moving one unit in the given direction from the given position.
    ///
    /// This method takes a direction such as Direction.west or an equivalent tuple such as (0, -1), but will not
    /// work with commands such as "w".
    ///
    /// - Parameter direction: The direction to move this position
    /// - Returns: The position moved one unit from the given direction
    func directionalOffset(_ direction: Direction) -> Position {
        // TODO
        return Position(x:0, y: 0)
    }
    
    /// This is a convenience method for directionalOffset(:) that takes a tuple instead of a direction.
    ///
    /// - Parameter tuple: An (x,y) tuple to offset the current position. It should represent one square in any
    ///   cardinal direction
    /// - Returns: The position moved one unit from the given position.
    func directionalOffset(tuple: (Int, Int)) -> Position {
        // TODO
        return Position(x: tuple.0, y: tuple.1)
    }
    
    /// Returns a list of all positions around the given position in each cardinal direction.
    ///
    /// - Returns: A list of all positions around the given position in each cardinal direction.
    func getSurroundingCardinals() -> [Position] {
        return []
    }
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
        return .stayStill
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
        return Direction.convert(direction)
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

/// Gameplay takes place on a wrapping rectangular grid 32x32, 40x40, 48x48, 56x56, or 64x64 in dimension. The map
/// edges wrap to their opposite edge and create a torus shape. The game map can be indexed by a position or by a
/// contained entity (ship, shipyard, or dropoff). The game map has width and height as member variables.
struct Map {
    let width: Int
    let height: Int
    
    /// A method that computes the Manhattan distance between two locations, and accounts for the toroidal wraparound.
    ///
    /// - Parameters:
    ///   - source: The position to measure from
    ///   - target: The position to measure to
    /// - Returns: The distance between the source and the target
    func calculateDistance(source: Position, target: Position) -> Int {
        return 0
    }
    
    /// A method that normalizes a position within the bounds of the toroidal map.
    ///
    /// Useful for handling the wraparound modulus arithmetic on x and y. For example, if a ship at (x = 31, y = 4)
    /// moves to the east on a 32x32 map, the normalized position would be (x = 0, y = 4), rather than the off-the-map
    /// position of (x = 32, y = 4).
    ///
    /// - Parameter position: The position to normalize
    /// - Returns: The normalized position.
    func normalize(position: Position) -> Position {
        return position
    }
    
    /// A method that returns a list of direction(s) to move closer to a target disregarding collision possibilities.
    /// Returns an empty list if the source and destination are the same.
    ///
    /// - Parameters:
    ///   - source: The position to move from
    ///   - destination: The position to move to
    /// - Returns: List of closest directions toward the given target.
    func getUnsafeMoves(source: Position, destination: Position) -> [Direction] {
        return []
    }
    
    /// A method that returns a direction to move closer to a target without colliding with other entities.
    ///
    /// Returns a direction of “still” if no such move exists.
    ///
    /// - Parameters:
    ///   - ship: The ship attempting to navigate
    ///   - destination: The ship's desired position.
    /// - Returns: A single valid direction toward a given target.
    func naiveNavigate(ship: Ship, destination: Position) -> Direction {
        return .still
    }
}

/// A map cell is an object representation of a cell on the game map.
struct MapCell {
    let position: Position
    let haliteAmount: Int
    let ship: Ship?
    let structure: String // what is this?
    
    /// Returns True if the cell is empty.
    ///
    /// - Returns: True if the cell is empty.
    func isEmpty() -> Bool {
        return false
    }
    
    /// Returns True if there is a ship on this cell.
    ///
    /// - Returns: True if there is a ship on this cell.
    func isOccupied() -> Bool {
        return ship != nil
    }
    
    /// Returns True if there is a structure (a dropoff or shipyard) on this cell.
    ///
    /// - Returns: True if there is a structure (a dropoff or shipyard) on this cell.
    func hasStructure() -> Bool {
        return false
    }
    
    /// Returns the type of structure on this cell, or nil if there is no structure.
    ///
    /// - Returns: Returns the type of structure on this cell, or nil if there is no structure.
    func structureType() -> String? {
        return nil
    }
    
    /// Used to mark the cell under this ship as unsafe (occupied) for collision avoidance.
    ///
    /// This marking resets every turn and is used by naive_navigate to avoid collisions.
    ///
    /// - Parameter ship: The ship on this cell.
    func markUnsafe(ship: Ship) {
        
    }
}
