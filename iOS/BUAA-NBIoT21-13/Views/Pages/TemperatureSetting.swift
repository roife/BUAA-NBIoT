//
//  TemperatureSettting.swift
//  BUAA-NBIoT21-13
//
//  Created by roife on 12/28/21.
//

import SwiftUI
import Combine

struct TemperatureSetting: View {
    @EnvironmentObject var device: DeviceVM
    @State var temperature: String = ""
    
    var body: some View {
        NavigationView {
            Form {
                TextField("报警温度", text: $temperature)
                    .keyboardType(.numberPad)
                    .onReceive(Just(temperature)) { newValue in
                        let filtered = newValue.filter { "0123456789".contains($0) }
                        if filtered != newValue {
                            self.temperature = filtered
                        }
                    }
                
                Button("绑定新设备") {
                    device.setTemperature(temperature: temperature)
                }
                .frame(maxWidth: .infinity, alignment: .center)
            }
            .navigationTitle("修改报警温度")
            .navigationBarTitleDisplayMode(.inline)
        }
    }
}

struct TemperatureSettting_Previews: PreviewProvider {
    static var previews: some View {
        TemperatureSetting()
    }
}
