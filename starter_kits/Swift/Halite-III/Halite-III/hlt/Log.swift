//
//  Log.swift
//  Halite-III
//
//  Created by Chris Downie on 10/24/18.
//  Copyright © 2018 Chris Downie. All rights reserved.
//
import Foundation

class Log {
    let filename: String
    private let logFile : FileHandle

    // MARK: Initialization
    init(playerId: Player.ID) {
        filename = "bot-\(playerId).log"
        
        // Create the file if it doesn't exist.
        let success = FileManager.default.createFile(atPath: filename, contents: nil, attributes: nil)
        assert(success, "File manager should be able to create that file.")
        
        logFile = FileHandle(forWritingAtPath: filename)!
    }
    deinit {
        logFile.closeFile()
    }

    // MARK: - Public methods
    func debug(_ string: String) {
        append(prefix: "DEBUG", message: string)
    }
    
    func info(_ string: String) {
        append(prefix: "INFO", message: string)
    }
    
    func error(_ string: String) {
        append(prefix: "ERROR", message: string)
    }
    
    private static var _shared: Log?
    static var shared: Log {
        precondition(_shared != nil)
        return _shared!
    }
    
    static func seedShared(playerId: Player.ID) -> Log {
        _shared = Log(playerId: playerId)
        return shared
    }

    // MARK: - Private methods
    private func append(prefix: String, message: String) {
        let string = "[\(prefix)] \(message)\n"
        logFile.write(string.data(using: .utf8)!)
    }
}
