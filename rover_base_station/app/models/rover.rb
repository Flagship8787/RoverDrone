class Rover < ActiveRecord::Base
	serialize :GPS, Hash
	attr_accessor :GPSData

	before_create :update_telemetry 
	before_save :update_telemetry

	TEL_DIR = File.expand_path(File.join(__FILE__, "../../../telemetry"))

	def telemetry_file
		return File.join(TEL_DIR, "rover_" + self.id.to_s + "tel.csv")
	end

	def update_telemetry
		# => First update the GPS
		self.update_gps_from_data

		# => Now save the whole bundle to telemetry file
		return if !File.exists?(TEL_DIR) || !File.directory?(TEL_DIR)
		
		telfile_obj = File.open(self.telemetry_file, "w+")

		telfile_obj.close
	end

	def update_gps_from_data
		puts "Updating GPS"

		return if self.GPSData.nil? || self.GPSData.length <= 0
		puts "There WAS GPS data!"


		gpsComps = self.GPSData.split(",")
		puts "There are " + gpsComps.count.to_s + " GPS data points"
		return if gpsComps.count != 13

		puts "There is a PROPER number of GPS Data points"


		gpsHash = Hash.new
		gpsHash["GPS Data"] 		= gpsComps[0]
		gpsHash["Time Stamp"] 		= gpsComps[1]
		gpsHash["Validity"] 		= gpsComps[2]
		gpsHash["Latitude"] 		= gpsComps[3]
		gpsHash["North/South"] 		= gpsComps[4]
		gpsHash["Longitude"] 		= gpsComps[5]
		gpsHash["East/West"] 		= gpsComps[6]
		gpsHash["Speed (in knots)"] = gpsComps[7]
		gpsHash["True course"] 		= gpsComps[8]
		gpsHash["Date stamp"] 		= gpsComps[9]
		gpsHash["Variation"] 		= gpsComps[10]
		gpsHash["Mode Indicator"] 	= gpsComps[11]
		gpsHash["E/W Checksum"] 	= gpsComps[12]

		self.GPS = gpsHash
	end
	def save_telemetry_to_file
	end
end