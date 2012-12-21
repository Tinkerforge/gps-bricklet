#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_gps'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'ABC' # Change to your UID

ipcon = IPConnection.new HOST, PORT # Create IP connection to brickd
gps = BrickletGPS.new UID # Create device object
ipcon.add_device gps # Add device to IP connection
# Don't use device before it is added to a connection

# Set Period for coordinates callback to 1s (1000ms)
# Note: The callback is only called every second if the 
#       coordinates have changed since the last call!
gps.set_coordinates_callback_period 1000

# Register current callback (parameter has unit mA)
gps.register_callback(BrickletGPS::CALLBACK_COORDINATES) do |latitude, ns, longitude, ew, pdop, hdop, vdop, epe|
  puts "Latitude: #{latitude/1000000.0} ° #{ns}"
  puts "Longitude: #{longitude/1000000.0} ° #{ew}"
end

puts 'Press key to exit'
$stdin.gets
ipcon.destroy