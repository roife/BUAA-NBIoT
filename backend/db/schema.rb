# This file is auto-generated from the current state of the database. Instead
# of editing this file, please use the migrations feature of Active Record to
# incrementally modify your database, and then regenerate this schema definition.
#
# Note that this schema.rb definition is the authoritative source for your
# database schema. If you need to create the application database on another
# system, you should be using db:schema:load, not running all the migrations
# from scratch. The latter is a flawed and unsustainable approach (the more migrations
# you'll amass, the slower it'll run and the greater likelihood for issues).
#
# It's strongly recommended that you check this file into your version control system.

ActiveRecord::Schema.define(version: 2021_11_27_031343) do

  create_table "crono_jobs", force: :cascade do |t|
    t.string "job_id", null: false
    t.text "log", limit: 1073741823
    t.datetime "last_performed_at"
    t.boolean "healthy"
    t.datetime "created_at", null: false
    t.datetime "updated_at", null: false
    t.index ["job_id"], name: "index_crono_jobs_on_job_id", unique: true
  end

  create_table "devices", force: :cascade do |t|
    t.string "app_key"
    t.string "app_secret"
    t.string "master_key"
    t.integer "product_id"
    t.string "device_id"
    t.string "device_name"
    t.integer "critical_temperature", default: 50, null: false
    t.datetime "created_at", null: false
    t.datetime "updated_at", null: false
  end

  create_table "records", force: :cascade do |t|
    t.float "temperature"
    t.float "humidity"
    t.integer "led0"
    t.integer "ir"
    t.integer "motor"
    t.integer "device_id"
    t.datetime "created_at", null: false
    t.datetime "updated_at", null: false
    t.index ["device_id"], name: "index_records_on_device_id"
  end

end
