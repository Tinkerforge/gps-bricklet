<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletGPS.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletGPS;

$host = 'localhost';
$port = 4223;
$uid = 'ABC'; // Change to your UID

$ipcon = new IPConnection(); // Create IP connection
$vc = new BrickletGPS($uid, ipcon); // Create device object

$ipcon->connect($host, $port); // Connect to brickd
// Don't use device before ipcon is connected

// Get current coordinates
$coords = $gps->getCoordinates();

echo "Latitude: " . $coords['latitude']/1000000.0 . "° " . $coords['ns'] . "\n";
echo "Longitude: " . $coords['longitude']/1000000.0 . "° " . $coords['ew'] . "\n";

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));
$ipcon->disconnect();

?>
