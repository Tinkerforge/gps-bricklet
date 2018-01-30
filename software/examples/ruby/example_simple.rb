#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_gps'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'XYZ' # Change XYZ to the UID of your GPS Bricklet

ipcon = IPConnection.new # Create IP connection
gps = BrickletGPS.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Get current coordinates as [latitude, ns, longitude, ew, pdop, hdop, vdop, epe]
coordinates = gps.get_coordinates

puts "Latitude: #{coordinates[0]/1000000.0} °"
puts "N/S: #{coordinates[1]}"
puts "Longitude: #{coordinates[2]/1000000.0} °"
puts "E/W: #{coordinates[3]}"

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
