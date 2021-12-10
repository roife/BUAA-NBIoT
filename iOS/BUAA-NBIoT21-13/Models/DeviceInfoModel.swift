//
//  Device.swift
//  BUAA-NBIoT21-13
//
//  Created by roife on 11/26/21.
//

import Foundation

struct DeviceInfoModel: Codable {
    let id: Int
    
    let appKey: String
    let appSecret: String
    let masterKey: String
    let productId: Int
    let deviceId: String
    
    let deviceName: String
}
