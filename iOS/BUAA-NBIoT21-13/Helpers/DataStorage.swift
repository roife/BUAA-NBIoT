//
//  DataStorage.swift
//  BUAA-NBIoT21-13
//
//  Created by roife on 11/28/21.
//

import Foundation

struct DataStorage {
    static let standard = UserDefaults.standard
    static let encoder = JSONEncoder()
    static let decoder = JSONDecoder()
    
    static let deviceInfoKey = "device"
    
    static func getDeviceInfo() -> DeviceInfoModel? {
        guard let data = standard.object(forKey: deviceInfoKey) as? Data else {
            return nil
        }
        
        guard let deviceInfo = try? decoder.decode(DeviceInfoModel.self, from: data) else {
            return nil
        }
        
        return deviceInfo
    }
    
    static func saveDeviceInfo(deviceInfo: DeviceInfoModel) {
        guard let encodedData = try? encoder.encode(deviceInfo) else {
            return
        }
        standard.set(encodedData, forKey: deviceInfoKey)
        standard.synchronize()
    }
}
