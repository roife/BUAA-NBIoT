class RecordsController < ApplicationController
  before_action :set_record, only: [:show]

  # GET /records/1
  def show
    render json: @record
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_record
      @record = Record.find(params[:id])
    end

    # Only allow a trusted parameter "white list" through.
    def record_params
      params.require(:record).permit(:temperature, :humidity, :led0, :ir, :motor, :device_id)
    end
end
