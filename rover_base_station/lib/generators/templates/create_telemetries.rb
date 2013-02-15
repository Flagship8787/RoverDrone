class CreateTelemetries < ActiveRecord::Migration
  def self.up
    create_table :telemetries do |t|
      t.integer :rover_id,    :null => true
      t.integer :Millis,    :null => true
      t.string  :GPSData,    :null => true
      t.integer :Clearance,    :null => true

      t.timestamps
    end
  end

  def self.down
    drop_table :telemetries
  end
end
