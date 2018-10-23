import Foundation

struct Constant: Codable {
    let SHIP_COST: Int // x
    let DROPOFF_COST: Int // x
    let MAX_HALITE: Int // x
    let MAX_TURNS: Int // x
    let EXTRACT_RATIO: Int // x
    let MOVE_COST_RATIO: Int // x
    let INSPIRATION_ENABLED: Bool// x
    let INSPIRATION_RADIUS: Int // x
    let INSPIRATION_SHIP_COUNT: Int // x
    let INSPIRED_EXTRACT_RATIO: Int // x
    let INSPIRED_BONUS_MULTIPLIER: Double // x
    let INSPIRED_MOVE_COST_RATIO: Int // x
}

class Game {
    var turnNumber = 0
    
    init() {
        let input = readLine(strippingNewline: false)!
        let data = input.data(using: .utf8)!
        let decoder = JSONDecoder()
        do {
            let constant = try decoder.decode(Constant.self, from: data)
            print("loaded, \(constant.INSPIRED_BONUS_MULTIPLIER)")
        } catch {
            print("\(error)")
        }
    }
}

let game = Game()
