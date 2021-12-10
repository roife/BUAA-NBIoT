//
//  StatusCard.swift
//  BUAA-NBIoT21-13
//
//  Created by roife on 11/25/21.
//

import SwiftUI

struct StatusCard: View {
    var title: String
    var value: String
    let symbol: String
    let backgroundColor: Color
    let enable: Bool
    
    var body: some View {
        HStack(alignment: .top) {
            VStack(alignment: .leading) {
                Text(title)
                    .font(.headline)
                    .padding(.bottom, 1)
                    .foregroundColor(enable ? .white : .primary)
                    
                Text(value)
                    .font(.title.bold())
                    .foregroundColor(enable ? .white : backgroundColor)
            }
             
            Spacer()
            

            Image(systemName: symbol)
                .font(.title)
                .foregroundColor(enable ? .white : .primary)
        }
        .padding(20)
        .background(enable ? backgroundColor : Color(.systemGray6))
        .cornerRadius(24)
    }
}

struct StatusCard_Previews: PreviewProvider {
    static var previews: some View {
        StatusCard(title: "实时温度", value: "50℃", symbol: "thermometer", backgroundColor: Color(.systemPink), enable: false)
        StatusCard(title: "实时温度", value: "50℃", symbol: "thermometer", backgroundColor: Color(.systemPink), enable: true)
    }
}
