#!/usr/bin/perl  

use Tinkerforge::IPConnection;
use Tinkerforge::BrickletGPS;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'f9b'; # Change to your UID

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $gps = Tinkerforge::BrickletGPS->new(&UID, $ipcon); # Create device object

# Callback function for coordinates
sub cb_coordinates
{
    my ($latitude, $ns, $longitude, $ew, $pdop, $hdop, $vdop, $epe) = @_;

    print "Latitude: ".$latitude/1000000.0.'° '.$ns."\n";
    print "Longitude: ".$longitude/1000000.0.'° '.$ew."\n";
}

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Set Period for coordinates callback to 1s (1000ms)
# Note: The callback is only called every second if the 
#       coordinates have changed since the last call!
$gps->set_coordinates_callback_period(1000);

# Register coordinates callback to function cb_coordinates
$gps->register_callback($gps->CALLBACK_COORDINATES, 'cb_coordinates');

print "Press any key to exit...\n";
<STDIN>;
$ipcon->disconnect();
