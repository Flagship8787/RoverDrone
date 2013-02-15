class CreateRovers < ActiveRecord::Migration
  def self.up
    create_table :rovers do |t|
      t.string    :Name,           :null => true
      t.text      :Description,    :null => true

      t.datetime  :Connected,      :null => true
      t.datetime  :TelemetryRcvd,  :null => true

      t.integer   :Millis,         :null => true
      t.text      :GPS,            :null => true
      t.integer   :Clearance,      :null => true

      t.timestamps
    end
  end

  def self.down
    drop_table :rovers
  end
end
