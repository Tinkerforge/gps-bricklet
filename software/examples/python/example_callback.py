#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change to your UID

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_gps import GPS

# Callback function for coordinates
def cb_coordinates(latitude, ns, longitude, ew, pdop, hdop, vdop, epe):
    print('Latitude: ' + str(latitude/1000000.0) + '° ' + ns)
    print('Longitude: ' + str(longitude/1000000.0) + '° ' + ew)

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    gps = GPS(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Set Period for coordinates callback to 1s (1000ms)
    # Note: The callback is only called every second if the 
    #       coordinates have changed since the last call!
    gps.set_coordinates_callback_period(1000)

    # Register coordinates callback to function cb_coordinates
    gps.register_callback(gps.CALLBACK_COORDINATES, cb_coordinates)

    raw_input('Press key to exit\n') # Use input() in Python 3
    ipcon.disconnect()
