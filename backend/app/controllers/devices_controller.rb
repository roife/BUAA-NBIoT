require Rails.root.join('app', 'helper','nbiot_helper.rb')

class DevicesController < ApplicationController
  before_action :set_device, only: [:show, :destroy, :start_warning, :stop_warning, :show_records]

  # GET /devices/1
  def show
    render json: @device
  end

  # POST /devices
  def create
    @device = Device.new(device_params)
    @device.device_name = NBIoTHelper.query_name(@device.app_key, @device.app_secret, @device.master_key, @device.product_id, @device.device_id)

    if @device.save
      render json: @device, status: :created, location: @device
    else
      render json: @device.errors, status: :unprocessable_entity
    end
  end

  # DELETE /devices/1
  def destroy
    @device.destroy
  end

  # POST /devices/1/warning
  def start_warning
    body = {:content => { :params => {:control_int => 1},
                          :serviceIdentifier => "motor_led0" },
            :deviceId => @device.device_id,
            :operator => "BUAA-NBIoT21-13",
            :productId => @device.product_id};

    res = NBIoTHelper.create_command(@device.app_key, @device.app_secret, @device.master_key, body)
    render json: res
  end

  # DELETE /devices/1/warning
  def stop_warning
    body = {:content => { :params => {:control_int => 0},
                          :serviceIdentifier => "motor_led0" },
            :deviceId => @device.device_id,
            :operator => "BUAA-NBIoT21-13",
            :productId => @device.product_id};

    res = NBIoTHelper.create_command(@device.app_key, @device.app_secret, @device.master_key, body)
    render json: res
  end

  # GET /devices/1/records
  def show_records
    render json: @device.records.last(params[:record_num])
  end

  # PUT /devices/1/critical_temperature
  def change_critical_temperature
    @device.update({:critical_temperature => device_params[:critical_temperature]})
    render json: @device, status: :updated, location: @device
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_device
      @device = Device.find(params[:id])
    end

    # Only allow a trusted parameter "white list" through.
    def device_params
      params.require(:device).permit(:app_key, :app_secret, :master_key, :product_id, :device_id, :critical_temperature)
    end
end
