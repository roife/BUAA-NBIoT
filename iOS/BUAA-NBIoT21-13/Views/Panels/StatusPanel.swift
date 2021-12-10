//
//  StatusPanel.swift
//  BUAA-NBIoT21-13
//
//  Created by roife on 11/26/21.
//

import SwiftUI

struct StatusPanel: View {
    @EnvironmentObject var device: DeviceVM
    
    var body: some View {
        VStack {
            HStack() {
                StatusCard(title: "温度",
                           value: DataFormatter.tempFormatter(temp: device.lastRecord.temperature),
                           symbol: "thermometer",
                           backgroundColor: Color(.systemPink),
                           enable: device.lastRecord.temperature >= 50)
                
                StatusCard(title: "湿度",
                           value: DataFormatter.humidityFormatter(humidity: device.lastRecord.humidity),
                           symbol: "drop",
                           backgroundColor: Color(.systemBlue),
                           enable: false)
            }
            .padding(.horizontal, 15)
            .padding(.bottom, 5)
            
            HStack() {
                StatusCard(title: "马达",
                           value: device.lastRecord.motor == .run ? "启动" : "停止",
                           symbol: "waveform",
                           backgroundColor: Color(.systemPurple),
                           enable: device.lastRecord.motor == .run)
                
                StatusCard(title: "红外",
                           value: device.lastRecord.ir == .block ? "遮挡" : "未遮挡",
                           symbol: "rays",
                           backgroundColor: Color(.systemIndigo),
                           enable: device.lastRecord.ir == .block)
            }
            .padding(.horizontal, 15)
        }
    }
}

struct StatusPanel_Previews: PreviewProvider {
    static var previews: some View {
        StatusPanel()
    }
}
