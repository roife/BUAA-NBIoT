//
//  DeviceVM.swift
//  BUAA-NBIoT21-13
//
//  Created by roife on 11/28/21.
//

import Foundation

enum DeviceStatus {
    case unconnected
    case normal
    case warning
}

class DeviceVM: ObservableObject {
    @Published var deviceInfo: DeviceInfoModel?
    @Published var records: [RecordModel] = []
    
    init(deviceInfo: DeviceInfoModel? = nil) {
        self.deviceInfo = deviceInfo
    }
    
    var deviceStatus: DeviceStatus {
        guard let lastRecord = records.last else {
            return .unconnected
        }
        
        return lastRecord.motor == .run ? .warning : .normal
    }
    
    var tempData: [Double] {
        return records.map{ $0.temperature } // fixme
    }
    
    var humidityData: [Double] {
        return records.map { $0.humidity } // fixme
    }
    
    var lastRecord: RecordModel {
        guard let lastRecord = records.last else {
            return RecordModel(temperature: 0, humidity: 0, ir: .notBlock, led0: .off, motor: .off, createdAt: "")
        }
        
        return lastRecord
    }
    
    func updateRecords() {
        guard let deviceInfo = self.deviceInfo else {
            return
        }
        
        let req = APIiot.getRecentRecord(id: deviceInfo.id, num: 40)
        req.response { res in
            switch res.result {
            case let .failure(err):
                print(err)
            case let .success(data):
                if let records = try? APIiot.decoder.decode([RecordModel].self, from: data!) {
                    self.records = records
                }
            }
        }
    }
    
    func startWarning() {
        guard let deviceInfo = self.deviceInfo else {
            return
        }
        
        let req = APIiot.startWarning(id: deviceInfo.id)
        req.response { res in
            switch res.result {
            case let .failure(err):
                print(err)
            case .success(_):
                break
            }
        }
    }
    
    func stopWarning() {
        guard let deviceInfo = self.deviceInfo else {
            return
        }
        
        let req = APIiot.stopWarning(id: deviceInfo.id)
        req.response { res in
            switch res.result {
            case let .failure(err):
                print(err)
            case .success(_):
                break
            }
        }
    }
}
