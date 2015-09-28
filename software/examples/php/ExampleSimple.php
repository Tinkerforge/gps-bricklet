<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletGPS.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletGPS;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change to your UID

$ipcon = new IPConnection(); // Create IP connection
$gps = new BrickletGPS(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Get current coordinates
$coordinates = $gps->getCoordinates();

echo "Latitude: " . $coordinates['latitude']/1000000.0 . "° " . $coordinates['ns'] . "\n";
echo "Longitude: " . $coordinates['longitude']/1000000.0 . "° " . $coordinates['ew'] . "\n";

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));
$ipcon->disconnect();

?>
