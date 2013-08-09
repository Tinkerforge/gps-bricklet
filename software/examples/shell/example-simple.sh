#!/bin/sh
# connects to localhost:4223 by default, use --host and --port to change it

# change to your UID
uid=XYZ

# get current coordinates
tinkerforge call gps-bricklet $uid get-coordinates
