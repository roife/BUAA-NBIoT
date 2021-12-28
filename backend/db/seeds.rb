# This file should contain all the record creation needed to seed the database with its default values.
# The data can then be loaded with the rails db:seed command (or created alongside the database with db:setup).
#
# Examples:
#
#   movies = Movie.create([{ name: 'Star Wars' }, { name: 'Lord of the Rings' }])
#   Character.create(name: 'Luke', movie: movies.first)

require Rails.root.join('app', 'helper','nbiot_helper.rb')

device = Device.create!({:app_key => "qrhj0fzeVR1", :app_secret => "MAaJDHu2l9", :master_key => "78ed9c4ca8c14c81b7ee86516cc9817e", :product_id => 15103201, :device_id => "1a58b602c2de43d1b8b01a8b9673c60a", :critical_temperature => 28})

res = NBIoTHelper.query_device_list("qrhj0fzeVR1", "MAaJDHu2l9", "78ed9c4ca8c14c81b7ee86516cc9817e", 15103201)
res = JSON.parse(res.body, symbolize_names: true)[:result][:list].find{|device| device[:deviceId] == "1a58b602c2de43d1b8b01a8b9673c60a"}
device.device_name = res[:deviceName]
device.save
puts res[:deviceName]