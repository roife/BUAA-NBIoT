//
//  HistoryPanel.swift
//  BUAA-NBIoT21-13
//
//  Created by roife on 11/26/21.
//

import SwiftUI
import SwiftUICharts

struct ChartPanel: View {
    @EnvironmentObject var device: DeviceVM
    
    var body: some View {
        let tempData = device.tempData
        let humidityData = device.humidityData
        
        VStack(alignment: .leading) {
            let historyImage = Image(systemName: "clock")
            Text("近期数据 \(historyImage)")
                .font(.system(.headline))
                .padding(.leading, 20)
                .foregroundColor(.secondary)
            
            ChartCard(titleSymbol: "thermometer",
                      title: "温度",
                      data: tempData,
                      startColor: .pink,
                      endColor: .red)
                .padding(15)
            
            ChartCard(titleSymbol: "drop",
                      title: "湿度",
                      data: humidityData,
                      startColor: .blue,
                      endColor: .purple)
                .padding(.horizontal, 15)
        }
    }
}

struct HistoryPanel_Previews: PreviewProvider {
    static var previews: some View {
        ChartPanel()
    }
}
