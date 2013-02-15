module RoverBaseStation
  module Generators
    class InstallGenerator < Rails::Generators::Base
      include Rails::Generators::Migration
      source_root File.expand_path("../../templates", __FILE__)

      desc "Creates an initializer and the necessary migration files."

      def copy_migrations
        migration_template "create_rovers.rb", "db/migrate/create_rovers.rb"
        #migration_template "create_telemetries.rb", "db/migrate/create_telemetries.rb"
      end

      def self.next_migration_number(path)
        unless @prev_migration_nr
          @prev_migration_nr = Time.now.utc.strftime("%Y%m%d%H%M%S").to_i
        else
          @prev_migration_nr += 1
        end
        @prev_migration_nr.to_s
      end

      def copy_initializer
        template "rover_base_station.rb", "config/initializers/rover_base_station.rb"
      end
    end
  end
end