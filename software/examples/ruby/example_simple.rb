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

# Get current coordinates
coords = gps.get_coordinates
puts "Latitude: #{coords[0]/1000000.0} ° #{coords[1]}"
puts "Longitude: #{coords[2]/1000000.0} ° #{coords[3]}"

puts 'Press key to exit'
$stdin.gets
ipcon.destroy
