import Foundation

class Game {
    var turnNumber = 0
    let log = Log(filename: "output.log")
    init() {
        let input = readLine(strippingNewline: false)!
        log.debug(input)
        let data = input.data(using: .utf8)!
        let decoder = JSONDecoder()
        do {
            let constant = try decoder.decode(Constant.self, from: data)
            log.debug("loaded, \(constant.INSPIRED_BONUS_MULTIPLIER)")
        } catch {
            log.error("\(error)")
        }
    }
}

let game = Game()
