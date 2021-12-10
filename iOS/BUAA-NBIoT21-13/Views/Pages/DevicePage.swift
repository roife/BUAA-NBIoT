//
//  DeviceData.swift
//  BUAA-NBIoT21-13
//
//  Created by roife on 11/25/21.
//

import SwiftUI

enum ActiveSheet: Identifiable {
    case deviceBiding, dataTable
    
    var id: Int {
        hashValue
    }
}

struct DevicePage: View {
    @EnvironmentObject var device: DeviceVM
    @State var activeSheet: ActiveSheet?
    
    var body: some View {
        NavigationView {
            ScrollView {
                StatusPanel().padding(.vertical, 15)
                ChartPanel()
            }
            .navigationTitle(device.deviceInfo?.deviceName ?? "未知设备")
            .toolbar {
                ToolbarItem(placement: .navigationBarLeading) {
                    DeviceStatusIndicator()
                }
                
                ToolbarItem(placement: .primaryAction) {
                    MainMenu(activeSheet: $activeSheet)
                }
            }
        }
        .navigationViewStyle(StackNavigationViewStyle())
        .sheet(item: $activeSheet) { item in
            switch item {
            case .deviceBiding:
                DeviceBindingPage(activeSheet: $activeSheet)
            case .dataTable:
                DataTablePage()
            }
        }
    }
}

struct DeviceData_Previews: PreviewProvider {
    static var previews: some View {
        DevicePage()
    }
}
