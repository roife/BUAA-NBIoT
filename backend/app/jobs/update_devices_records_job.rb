require 'nbiot_helper'

class UpdateDevicesRecordsJob < ApplicationJob
  queue_as :default

  def perform
    Device.all.each{|device|
      res = NBIoTHelper.query_device_status_list(device.app_key, device.app_secret,
        {:productId => device.product_id,
          :deviceId => device.device_id
        })

      status = JSON.parse(res.body, symbolize_names: true)[:deviceStatusList]
      data = NBIoTHelper.analysis_status(status)
      data.update({:device => device})
      Record.create!(data)
    }
  end
end
