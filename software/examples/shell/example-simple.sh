#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your GPS Bricklet

# Get current coordinates
tinkerforge call gps-bricklet $uid get-coordinates
