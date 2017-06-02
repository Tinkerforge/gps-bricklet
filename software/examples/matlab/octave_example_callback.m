function octave_example_callback()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your GPS Bricklet

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    gps = javaObject("com.tinkerforge.BrickletGPS", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register coordinates callback to function cb_coordinates
    gps.addCoordinatesCallback(@cb_coordinates);

    % Set period for coordinates callback to 1s (1000ms)
    % Note: The coordinates callback is only called every second
    %       if the coordinates has changed since the last call!
    gps.setCoordinatesCallbackPeriod(1000);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end

% Callback function for coordinates callback
function cb_coordinates(e)
    fprintf("Latitude: %g° %s\n", java2int(e.latitude)/1000000.0, e.ns);
    fprintf("Longitude: %g° %s\n", java2int(e.longitude)/1000000.0, e.ew);
    fprintf("\n");
end

function int = java2int(value)
    if compare_versions(version(), "3.8", "<=")
        int = value.intValue();
    else
        int = value;
    end
end
