#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change to your UID

# Handle incoming coordinates callbacks
tinkerforge dispatch gps-bricklet $uid coordinates &

# Set period for coordinates callback to 1s (1000ms)
# Note: The coordinates callback is only called every second
#       if the coordinates has changed since the last call!
tinkerforge call gps-bricklet $uid set-coordinates-callback-period 1000

echo "Press key to exit"; read dummy

kill -- -$$ # Stop callback dispatch in background
