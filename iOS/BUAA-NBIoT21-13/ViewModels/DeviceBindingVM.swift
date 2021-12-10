//
//  DeviceBindingVM.swift
//  BUAA-NBIoT21-13
//
//  Created by roife on 11/28/21.
//

import Foundation

class DeviceBindingVM: ObservableObject {
    @Published var showProgressView = false
    var envDevice: DeviceVM?
    
    func createDevice(appKey: String, appSecret: String, masterKey: String, productId: String, deviceId: String) {
        self.showProgressView = true
        
        if let oldDevice = envDevice!.deviceInfo {
            APIiot.deleteDevice(id: oldDevice.id).response { res in
                debugPrint(res)
            }
        }
        
        let reqLogin = APIiot.createDeviceReq(appKey: appKey,
                                              appSecret: appSecret,
                                              masterKey: masterKey,
                                              productId: productId,
                                              deviceId: deviceId)

        reqLogin.response { res in
            switch res.result {
            case let .failure(err):
                print(err)
            case let .success(data):
                if let deviceInfo = try? APIiot.decoder.decode(DeviceInfoModel.self, from: data!) {
                    self.envDevice!.deviceInfo = deviceInfo
                    DataStorage.saveDeviceInfo(deviceInfo: deviceInfo)
                }
            }
            self.showProgressView = false
        }
    }
}
