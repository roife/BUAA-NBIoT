//
//  DeviceStatus.swift
//  BUAA-NBIoT21-13
//
//  Created by roife on 11/26/21.
//

import SwiftUI

struct DeviceStatusIndicator: View {
    @EnvironmentObject var device: DeviceVM
    
    var body: some View {
        let status = indicatorStatus()
        
        HStack {
            Image(systemName: "circle.fill")
                .font(.system(size: 8))
                .foregroundColor(status.0)
            Text(status.1)
                .font(.system(.headline))
                .foregroundColor(Color(.systemGray2))
        }
    }
    
    func indicatorStatus() -> (Color, String) {
        switch device.deviceStatus {
        case .unconnected:
            return (.orange, "未连接")
        case .normal:
            return (.green, "正常")
        case .warning:
            return (.red, "警报")
        }
    }
}

struct DeviceStatusIndicator_Previews: PreviewProvider {
    static var previews: some View {
        DeviceStatusIndicator()
    }
}
