//
//  ContentView.swift
//  BUAA-NBIoT21-13
//
//  Created by roife on 11/25/21.
//

import SwiftUI

struct ContentView: View {
    let device = DeviceVM(deviceInfo: DataStorage.getDeviceInfo())
    let dataFetcherTimer = Timer.publish(every: 1, on: .main, in: .common).autoconnect()
    
    var body: some View {
        DevicePage()
            .environmentObject(device)
            .onReceive(dataFetcherTimer) { _ in
                device.updateRecords()
            }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
