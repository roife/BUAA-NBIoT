//
//  ChartCard.swift
//  BUAA-NBIoT21-13
//
//  Created by roife on 11/26/21.
//

import SwiftUI
import SwiftUICharts

struct ChartCard: View {
    let titleSymbol: String
    let title: String
    var data: [Double]
    let startColor: Color
    let endColor: Color
    
    var body: some View {
        VStack(alignment: .center) {
            ChartGrid {
                VStack {
                    HStack {
                        Image(systemName: titleSymbol)
                            .font(.title)
                            .foregroundColor(Color(.systemGray2))
                        Text(title)
                            .foregroundColor(Color(.systemGray2))
                        
                        Spacer()
                        
                        let chartLabelType = ChartLabelType.custom(size: 30,
                                                                   padding: EdgeInsets(),
                                                                   color: startColor)
                        ChartLabel(" ", type:chartLabelType)
                    }
                    .padding(.top, 17)
                    .padding(.horizontal, 20)
                    
                    LineChart()
                        .frame(height: 150)
                        .padding(.bottom, 10)
                }
            }
            .data(data)
            .chartStyle(ChartStyle(backgroundColor: Color(.systemBackground),
                                   foregroundColor: ColorGradient(startColor, endColor)))
            .padding(-3)
        }
        .background(Color(.systemBackground))
        .cornerRadius(24)
        .shadow(color: Color(.systemGray4), radius: 4, y: 1)
    }
}

struct ChartCard_Previews: PreviewProvider {
    static var previews: some View {
        ChartCard(titleSymbol:"thermometer", title:"温度", data: [3, 5, 2, 5, 4, 6, 7, 8, 5, 9, 6, 8, 4, 6, 7], startColor: .pink, endColor: .red)
    }
}
