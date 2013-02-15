require 'rails'
require 'rover_base_station/engine'
#require "rover_base_station/version"

module RoverBaseStation
	def self.setup
    yield self
  end
end
