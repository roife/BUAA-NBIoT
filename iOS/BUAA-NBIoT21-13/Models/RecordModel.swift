//
//  Record.swift
//  BUAA-NBIoT21-13
//
//  Created by roife on 11/26/21.
//

import Foundation

enum IrStatus: Int, Codable {
    case block = 0, notBlock = 1
}

enum LedStatus: Int, Codable {
    case off = 0, green = 1, red = 2, blue = 4, purple = 6, yellow = 3, paleBlue = 5, white = 7
}

enum MotorStatus: Int, Codable {
    case off = 0, run, backUp
}

struct RecordModel: Codable {
    let temperature: Double
    let humidity: Double
    let ir: IrStatus
    let led0: LedStatus
    let motor: MotorStatus
    let createdAt: String
}
