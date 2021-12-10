//
//  DataPanel.swift
//  BUAA-NBIoT21-13
//
//  Created by roife on 11/26/21.
//

import SwiftUI

struct DataTablePage: View {
    @EnvironmentObject var device: DeviceVM
    
    var body: some View {
        NavigationView {
            VStack(alignment: .leading) {
                List {
                    let header = Text("温湿度数据")
                        .font(.subheadline)
                        .padding(.top, 10)
                        .padding(.bottom, 5)
                    
                    Section(header: header) {
                        ForEach(device.records, id: \.createdAt) { record in
                            let temp = record.temperature
                            let humidity = record.humidity
                            
                            HStack {
                                Image(systemName: "thermometer")
                                    .foregroundColor(.pink)
                                Text(DataFormatter.tempFormatter(temp: temp))
                                    .foregroundColor(.pink)
                                
                                Image(systemName: "drop")
                                    .foregroundColor(.blue)
                                Text(DataFormatter.humidityFormatter(humidity: humidity))
                                    .foregroundColor(.blue)
                                
                                Spacer()
                                
//                                Text(DataFormatter.shortenDate(date: record.timestamp))
//                                    .foregroundColor(.gray)
                            }
                        }
                    }
                }
                .listStyle(InsetGroupedListStyle())
            }
            .navigationTitle("所有数据")
            .navigationBarTitleDisplayMode(.inline)
        }
    }
}

struct DataPanel_Previews: PreviewProvider {
    static var previews: some View {
        DataTablePage()
    }
}
