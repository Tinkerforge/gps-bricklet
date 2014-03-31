function octave_example_callback
    more off;
    
    HOST = "localhost";
    PORT = 4223;
    UID = "ABC"; % Change to your UID

    ipcon = java_new("com.tinkerforge.IPConnection"); % Create IP connection
    gps = java_new("com.tinkerforge.BrickletGPS", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Set Period for coordinates callback to 1s (1000ms)
    % Note: The callback is only called every second if the 
    %       coordinates have changed since the last call!
    gps.setCoordinatesCallbackPeriod(1000);

    % Register coordinates callback to function cb_coordinates
    gps.addCoordinatesListener("cb_coordinates");

    input("\nPress any key to exit...\n", "s");
    ipcon.disconnect();
end

% Callback function for coordinates
function cb_coordinates(latitude, ns, longitude, ew, pdop, hdop, vdop, epe)
    latitude = str2num(latitude.toString());
    longitude = str2num(longitude.toString());
    fprintf("Latitude: %g° %s\n", latitude/1000000, ns);
    fprintf("Longitude: %g° %s\n", longitude/1000000, ew);
end
