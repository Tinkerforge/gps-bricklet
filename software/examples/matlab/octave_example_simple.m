function octave_example_simple()
    more off;
    
    HOST = "localhost";
    PORT = 4223;
    UID = "ABC"; % Change to your UID

    ipcon = java_new("com.tinkerforge.IPConnection"); % Create IP connection
    gps = java_new("com.tinkerforge.BrickletGPS", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Get current coordinates
    coords = gps.getCoordinates();
    latitude = str2num(coords.latitude.toString());
    longitude = str2num(coords.longitude.toString());
    fprintf("Latitude: %g° %s\n", latitude/1000000.0, coords.ns);
    fprintf("Longitude: %g° %s\n", longitude/1000000.0, coords.ew);

    input("Press any key to exit...\n", "s");
    ipcon.disconnect();
end
