import Foundation

enum Command {
    enum MoveDirection: String {
        case north = "n"
        case west = "w"
        case south = "s"
        case east = "e"
    }
    
    case generate
    case construct
    case move(direction: MoveDirection)
    
    static func string(for command: Command) -> String {
        switch command {
        case .generate:
            return "g"
        case .construct:
            return "c"
        case .move(let direction):
            return "m\(direction.rawValue)"
        }
    }
}
