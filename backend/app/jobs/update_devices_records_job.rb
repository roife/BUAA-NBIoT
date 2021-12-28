require 'nbiot_helper'

class UpdateDevicesRecordsJob < ApplicationJob
  queue_as :default

  def perform
    Device.all.each{|device|
      res = NBIoTHelper.query_device_status_list(
              device.app_key,
              device.app_secret,
              {
                :productId => device.product_id,
                :deviceId => device.device_id
              }
            )

      status = JSON.parse(res.body, symbolize_names: true)[:deviceStatusList]
      data = NBIoTHelper.analysis_status(status)
      data.update({:device => device})
      if data[:temperature].to_f > device.critical_temperature
        body = {
                :content => { :params => {:control_int => 1},
                              :serviceIdentifier => "motor_led0",
                            },
                :deviceId => device.device_id,
                :operator => "BUAA-NBIoT21-13",
                :productId => device.product_id
              };

        res = NBIoTHelper.create_command(device.app_key, device.app_secret, device.master_key, body)
      end
      Record.create!(data)
    }
  end
end
