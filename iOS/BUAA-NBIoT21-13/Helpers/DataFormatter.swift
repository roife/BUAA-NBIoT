//
//  Formatters.swift
//  BUAA-NBIoT21-13
//
//  Created by roife on 11/27/21.
//

import Foundation

struct DataFormatter {
    static func shortenDate(date: Date) -> String {
        let toFormatter = DateFormatter()
        toFormatter.dateFormat = "MMM dd, HH:mm:ss"
        
        return toFormatter.string(from: date)
    }
    
    static func tempFormatter(temp: Double) -> String {
        String(format: "%.1f℃", temp)
    }
    
    static func humidityFormatter(humidity: Double) -> String {
        String(format: "%.1f%%", humidity)
    }
}
