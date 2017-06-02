function octave_example_simple()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your GPS Bricklet

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    gps = javaObject("com.tinkerforge.BrickletGPS", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Get current coordinates
    coordinates = gps.getCoordinates();

    fprintf("Latitude: %g° %s\n", java2int(coordinates.latitude)/1000000.0, coordinates.ns);
    fprintf("Longitude: %g° %s\n", java2int(coordinates.longitude)/1000000.0, coordinates.ew);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end

function int = java2int(value)
    if compare_versions(version(), "3.8", "<=")
        int = value.intValue();
    else
        int = value;
    end
end
