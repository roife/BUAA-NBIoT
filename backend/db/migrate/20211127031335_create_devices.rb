class CreateDevices < ActiveRecord::Migration[5.2]
  def change
    create_table :devices do |t|
      t.string :app_key
      t.string :app_secret
      t.string :master_key
      t.integer :product_id
      t.string :device_id
      t.string :device_name

      t.timestamps
    end
  end
end
