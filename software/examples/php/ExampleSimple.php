<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletGPS.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletGPS;

$host = 'localhost';
$port = 4223;
$uid = 'ABC'; // Change to your UID

$ipcon = new IPConnection($host, $port); // Create IP connection to brickd
$gps = new BrickletGPS($uid); // Create device object

$ipcon->addDevice($gps); // Add device to IP connection
// Don't use device before it is added to a connection

// Get current coordinates
$coords = $gps->getCoordinates();

echo "Latitude: " . $coords['latitude']/1000000.0 . "° " . $coords['ns'] . "\n";
echo "Longitude: " . $coords['longitude']/1000000.0 . "° " . $coords['ew'] . "\n";

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));
$ipcon->destroy();

?>
