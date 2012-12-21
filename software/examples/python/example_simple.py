#!/usr/bin/env python
# -*- coding: utf-8 -*-  

HOST = "localhost"
PORT = 4223
UID = "ABC" # Change to your UID

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_gps import GPS

if __name__ == "__main__":
    ipcon = IPConnection(HOST, PORT) # Create ip connection to brickd

    gps = GPS(UID) # Create device object
    ipcon.add_device(gps) # Add device to ip connection
    # Don't use device before it is added to a connection

    # Get current current and voltage (unit is mA and mV)
    coords = gps.get_coordinates()

    print('Latitude: ' + str(coords.latitude/1000000.0) + '° ' + coords.ns)
    print('Longitude: ' + str(coords.longitude/1000000.0) + '° ' + coords.ew)

    raw_input('Press key to exit\n') # Use input() in Python 3
    ipcon.destroy()
