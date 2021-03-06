#!/usr/bin/perl

use strict;
use Tinkerforge::IPConnection;
use Tinkerforge::BrickletGPS;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your GPS Bricklet

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $gps = Tinkerforge::BrickletGPS->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Get current coordinates
my ($latitude, $ns, $longitude, $ew, $pdop, $hdop, $vdop, $epe) = $gps->get_coordinates();

print "Latitude: " . $latitude/1000000.0 . " °\n";
print "N/S: $ns\n";
print "Longitude: " . $longitude/1000000.0 . " °\n";
print "E/W: $ew\n";

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
