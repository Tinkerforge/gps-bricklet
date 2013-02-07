#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_gps'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'ABC' # Change to your UID

ipcon = IPConnection.new # Create IP connection
gps = BrickletGPS.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Get current coordinates
coords = gps.get_coordinates
puts "Latitude: #{coords[0]/1000000.0}° #{coords[1]}"
puts "Longitude: #{coords[2]/1000000.0}° #{coords[3]}"

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
