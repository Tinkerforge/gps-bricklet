#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change XYZ to the UID of your GPS Bricklet

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_gps import BrickletGPS

# Callback function for coordinates callback
def cb_coordinates(latitude, ns, longitude, ew, pdop, hdop, vdop, epe):
    print("Latitude: " + str(latitude/1000000.0) + " °")
    print("N/S: " + ns)
    print("Longitude: " + str(longitude/1000000.0) + " °")
    print("E/W: " + ew)
    print("")

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    gps = BrickletGPS(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Register coordinates callback to function cb_coordinates
    gps.register_callback(gps.CALLBACK_COORDINATES, cb_coordinates)

    # Set period for coordinates callback to 1s (1000ms)
    # Note: The coordinates callback is only called every second
    #       if the coordinates has changed since the last call!
    gps.set_coordinates_callback_period(1000)

    raw_input("Press key to exit\n") # Use input() in Python 3
    ipcon.disconnect()
