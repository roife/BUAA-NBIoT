class CreateRecords < ActiveRecord::Migration[5.2]
  def change
    create_table :records do |t|
      t.float :temperature
      t.float :humidity
      t.integer :led0
      t.integer :ir
      t.integer :motor
      t.belongs_to :device, foreign_key: true

      t.timestamps
    end
  end
end
