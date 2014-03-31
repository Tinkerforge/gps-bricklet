function matlab_example_simple
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletGPS;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'ABC'; % Change to your UID
    
    ipcon = IPConnection(); % Create IP connection
    gps = BrickletGPS(UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Get current coordinates
    coords = gps.getCoordinates();

    fprintf('Latitude: %g° %s\n', coords.latitude/1000000, coords.ns);
    fprintf('Longitude: %g° %s\n', coords.longitude/1000000, coords.ew);

    input('\nPress any key to exit...\n', 's');
    ipcon.disconnect();
end


