<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletGPS.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletGPS;

$host = 'localhost';
$port = 4223;
$uid = 'ABC'; // Change to your UID

// Callback function for current callback (parameter has unit mA)
function cb_coordinates($latitude, $ns, $longitude, $ew, $pdop, $hdop, $vdop, $epe)
{
	echo "Latitude: " . $latitude/1000000.0 . "° " . $ns . "\n";
	echo "Longitude: " . $longitude/1000000.0 . "° " . $ew . "\n";
}

$ipcon = new IPConnection($host, $port); // Create IP connection to brickd
$vc = new BrickletGPS($uid); // Create device object

$ipcon->addDevice($vc); // Add device to IP connection
// Don't use device before it is added to a connection

// Set Period for coordinates callback to 1s (1000ms)
// Note: The callback is only called every second if the 
//       coordinates have changed since the last call!
$vc->setCoordinatesCallbackPeriod(1000);

// Register current callback to function cb_coordinates
$vc->registerCallback(BrickletGPS::CALLBACK_COORDINATES, 'cb_coordinates');

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
