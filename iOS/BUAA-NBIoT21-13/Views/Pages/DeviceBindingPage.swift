//
//  DeviceBinding.swift
//  BUAA-NBIoT21-13
//
//  Created by roife on 11/26/21.
//

import SwiftUI

struct DeviceBindingPage: View {
    @EnvironmentObject var device: DeviceVM
    @ObservedObject var bindingVM = DeviceBindingVM()
    @Binding var activeSheet: ActiveSheet?
    
    @State var appKey: String = ""
    @State var appSecret: String = ""
    @State var masterKey: String = ""
    @State var productId: String = ""
    @State var deviceId: String = ""
    
    var body: some View {
        if bindingVM.showProgressView {
            ProgressView("正在绑定")
                .progressViewStyle(CircularProgressViewStyle())
                .onDisappear {
                    self.activeSheet = nil
                }
        } else {
            NavigationView {
                Form {
                    Section(header: Text("产品信息")) {
                        InfoTextField(title: "AppKey", binder: $appKey)
                        InfoTextField(title: "AppSecret", binder: $appSecret)
                        InfoTextField(title: "MasterKey", binder: $masterKey)
                        InfoTextField(title: "ProductId", binder: $productId)
                    }
                    
                    Section(header: Text("设备信息")) {
                        InfoTextField(title: "DeviceId", binder: $deviceId)
                    }
                    
                    Section {
                        Button("绑定新设备") {
                            bindingVM.createDevice(appKey: appKey,
                                                   appSecret: appSecret,
                                                   masterKey: masterKey,
                                                   productId: productId,
                                                   deviceId: deviceId)
                        }
                        .frame(maxWidth: .infinity, alignment: .center)
                    }
                    .disabled(appKey.isEmpty || appSecret.isEmpty || masterKey.isEmpty || productId.isEmpty || deviceId.isEmpty)
                }
                .navigationTitle("绑定新设备")
                .navigationBarTitleDisplayMode(.inline)
            }
            .onAppear {
                self.bindingVM.envDevice = device
            }
        }
    }
    
    func InfoTextField(title: String, binder: Binding<String>) -> some View {
        TextField("\(title)", text: binder)
            .disableAutocorrection(true)
            .autocapitalization(.none)
    }
}

struct DeviceBinding_Previews: PreviewProvider {
    static var previews: some View {
        DeviceBindingPage(activeSheet: .constant(nil))
    }
}
