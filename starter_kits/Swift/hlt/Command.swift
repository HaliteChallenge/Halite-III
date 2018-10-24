import Foundation

enum Direction: String {
    case north = "n"
    case west = "w"
    case south = "s"
    case east = "e"
}

enum Command {
    case generate
    case construct
    case move(direction: Direction)
    
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
