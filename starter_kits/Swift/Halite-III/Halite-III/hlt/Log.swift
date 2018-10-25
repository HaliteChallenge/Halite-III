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

    init(filename: String) {
        self.filename = filename
        logFile = FileHandle(forWritingAtPath: filename)!
    }
    deinit {
        logFile.closeFile()
    }

    func debug(_ string: String) {
        append(prefix: "DEBUG", message: string)
    }
    func info(_ string: String) {
        append(prefix: "INFO", message: string)
    }
    func error(_ string: String) {
        append(prefix: "ERROR", message: string)
    }

    private func append(prefix: String, message: String) {
        let string = "[\(prefix)] \(message)"
        logFile.write(string.data(using: .utf8)!)
    }
}
