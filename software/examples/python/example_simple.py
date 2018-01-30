#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change XYZ to the UID of your GPS Bricklet

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_gps import BrickletGPS

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    gps = BrickletGPS(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Get current coordinates
    latitude, ns, longitude, ew, pdop, hdop, vdop, epe = gps.get_coordinates()

    print("Latitude: " + str(latitude/1000000.0) + " °")
    print("N/S: " + ns)
    print("Longitude: " + str(longitude/1000000.0) + " °")
    print("E/W: " + ew)

    raw_input("Press key to exit\n") # Use input() in Python 3
    ipcon.disconnect()
