//
//  IoTRequest.swift
//  BUAA-NBIoT21-13
//
//  Created by roife on 11/27/21.
//

import Foundation
import Alamofire

struct APIiot {
    static let host = "http://49.232.173.163:19373"
    static let decoder: JSONDecoder = {
        let decoder = JSONDecoder()
        decoder.keyDecodingStrategy = .convertFromSnakeCase
        return decoder
    }()
    
    static func getRecentRecord(id: Int, num: Int) -> DataRequest {
        let path = "/devices/\(id)/records"
        let url = host + path
        
        let params: [String: Int] = [
            "record_num": num
        ]
        
        return AF.request(url, method: .get, parameters: params)
    }
    
    static func setTemperature(id: Int, temperature: Int) -> DataRequest {
        let path = "/devices/\(id)/critical_temperature"
        let url = host + path
        
        let params: [String: Int] = [
            "critical_temperature": temperature,
        ]
        
        let req = AF.request(url, method: .put, parameters: params, encoding: JSONEncoding.default)
        return req
    }
    
    static func startWarning(id: Int) -> DataRequest {
        let path = "/devices/\(id)/warning"
        let url = host + path
                
        return AF.request(url, method: .post)
    }
    
    static func stopWarning(id: Int) -> DataRequest {
        let path = "/devices/\(id)/warning"
        let url = host + path
                
        return AF.request(url, method: .delete)
    }
    
    static func getDeviceInfo(id: Int) -> DataRequest {
        let path = "/devices/\(id)"
        let url = host + path
                
        return AF.request(url, method: .get)
    }
    
    static func createDeviceReq(appKey: String, appSecret: String, masterKey: String, productId: String, deviceId: String) -> DataRequest {
        let path = "/devices"
        let url = host + path
        
        let params: [String: String] = [
            "app_key": appKey,
            "app_secret": appSecret,
            "master_key": masterKey,
            "product_id": productId,
            "device_id": deviceId
        ]
        
        let req = AF.request(url, method: .post, parameters: params, encoding: JSONEncoding.default)
        return req
    }
    
    static func deleteDevice(id: Int) -> DataRequest {
        let path = "/devices/\(id)"
        let url = host + path
        
        return AF.request(url, method: .delete)
    }
}
