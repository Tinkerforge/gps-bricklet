var Tinkerforge = require('tinkerforge');

var HOST = 'localhost';
var PORT = 4223;
var UID = 'ABC'; // Change to your UID

var ipcon = new Tinkerforge.IPConnection(); // Create IP connection
var gps = new Tinkerforge.BrickletGPS(UID, ipcon); // Create device object

ipcon.connect(HOST, PORT,
    function(error) {
        console.log('Error: '+error);
    }
); // Connect to brickd
// Don't use device before ipcon is connected

ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function(connectReason) {
        // Set Period for coordinates callback to 1s (1000ms)
        // Note: The callback is only called every second if the
        // coordinates have changed since the last call!
        gps.setCoordinatesCallbackPeriod(1000);
    }
);

// Register coordinates callback
gps.on(Tinkerforge.BrickletGPS.CALLBACK_COORDINATES,
    // Callback function for coordinates
    function(latitude, ns, longitude, ew) {
        console.log('Latitude: '+latitude/1000000+'° '+ns);
        console.log('Longitude: '+longitude/1000000+'° '+ew);
        console.log();
    }
);

console.log("Press any key to exit ...");
process.stdin.on('data',
    function(data) {
        ipcon.disconnect();
        process.exit(0);
    }
);
