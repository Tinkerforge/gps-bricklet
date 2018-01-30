var Tinkerforge = require('tinkerforge');

var HOST = 'localhost';
var PORT = 4223;
var UID = 'XYZ'; // Change XYZ to the UID of your GPS Bricklet

var ipcon = new Tinkerforge.IPConnection(); // Create IP connection
var gps = new Tinkerforge.BrickletGPS(UID, ipcon); // Create device object

ipcon.connect(HOST, PORT,
    function (error) {
        console.log('Error: ' + error);
    }
); // Connect to brickd
// Don't use device before ipcon is connected

ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function (connectReason) {
        // Get current coordinates
        gps.getCoordinates(
            function (latitude, ns, longitude, ew, pdop, hdop, vdop, epe) {
                console.log('Latitude: ' + latitude/1000000.0 + ' °');
                console.log('N/S: ' + ns);
                console.log('Longitude: ' + longitude/1000000.0 + ' °');
                console.log('E/W: ' + ew);
            },
            function (error) {
                console.log('Error: ' + error);
            }
        );
    }
);

console.log('Press key to exit');
process.stdin.on('data',
    function (data) {
        ipcon.disconnect();
        process.exit(0);
    }
);
