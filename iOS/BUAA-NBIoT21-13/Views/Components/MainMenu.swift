//
//  Menu.swift
//  BUAA-NBIoT21-13
//
//  Created by roife on 11/26/21.
//

import SwiftUI

struct MainMenu: View {
    @EnvironmentObject var device: DeviceVM
    @Binding var activeSheet: ActiveSheet?
    
    var body: some View {
        Menu {
            Section {
                Button() {
                    self.activeSheet = .dataTable
                } label: {
                    Label("查看所有数据", systemImage: "tablecells.badge.ellipsis")
                }
                
                Button() {
                    switch device.deviceStatus {
                    case .warning:
                        device.stopWarning()
                    case .normal:
                        device.startWarning()
                    case .unconnected:
                        break
                    }
                } label: {
                    Label(device.deviceStatus == .warning ? "停止警报" : "开启警报", systemImage: "bell")
                }
                .disabled(device.deviceStatus == .unconnected)
            }
            
            Section {
                Button() {
                    self.activeSheet = .deviceBiding
                } label: {
                    Label("绑定新设备", systemImage: "arrow.2.squarepath")
                }
            }
        } label: {
            Image(systemName: "ellipsis")
                .font(.title2)
                .foregroundColor(.blue)
        }
    }
}

struct Menu_Previews: PreviewProvider {
    static var previews: some View {
        MainMenu(activeSheet: .constant(nil))
    }
}
