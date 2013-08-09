#!/bin/sh
# connects to localhost:4223 by default, use --host and --port to change it

# change to your UID
uid=XYZ

# set period for coordinates callback to 1s (1000ms)
# note: the coordinates callback is only called every second if the
#       coordinates have changed since the last call!
tinkerforge call gps-bricklet $uid set-coordinates-callback-period 1000

# handle incoming coordinates callbacks
tinkerforge dispatch gps-bricklet $uid coordinates
