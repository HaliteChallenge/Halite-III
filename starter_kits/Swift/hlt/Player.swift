import Foundation

struct Player {
    let id: String
//    let shipyard: Shipyard
    let haliteAmount: Int
    
    // MARK: - Ships
    func get(ship: Ship.ID) -> Ship? {
        return nil
    }
    func getShips() -> [Ship] {
        return []
    }
    func has(ship: Ship.ID) -> Bool {
        return false
    }
    
    // MARK: - Dropoffs
//    func get(dropoff: Dropoff.ID) -> Dropoff {
//
//    }
//
//    func getDropoffs() -> [Dropoff] {
//
//    }
}

struct Ship {
    typealias ID = String
    
    let owner: Int // what is this?
    let id: ID
    let position: (Int, Int) // what is this?
    let haliteAmount: Int
    
    func isFull() -> Bool {
        return haliteAmount >= Constant.shared.MAX_ENERGY
    }
    
    func makeDropoff() -> Command {
        return .construct
    }
    
    func stayStill() -> Command {
        return .move(direction: .north)
    }
    
    func move(direction: Direction) -> Command {
        return .move(direction: direction)
    }
}

struct Shipyard {
    let owner: String
    let id: String
    let position: (Int, Int)
}
