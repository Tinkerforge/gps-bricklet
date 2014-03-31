function matlab_example_callback
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletGPS;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'ABC'; % Change to your UID
    
    ipcon = IPConnection(); % Create IP connection
    gps = BrickletGPS(UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Set Period for coordinates callback to 1s (1000ms)
    % Note: The callback is only called every second if the 
    %       coordinates have changed since the last call!
    gps.setCoordinatesCallbackPeriod(1000);

    % Register coordinates callback to function cb_coordinates
    set(gps, 'CoordinatesCallback', @(h, e)cb_coordinates(e.latitude, e.ns, e.longitude, e.ew, e.pdop, e.hdop, e.vdop, e.epe));

    input('\nPress any key to exit...\n', 's');
    ipcon.disconnect();
end

% Callback function for coordinates
function cb_coordinates(latitude, ns, longitude, ew, pdop, hdop, vdop, epe)
    fprintf('Latitude: %g° %s\n', latitude/1000000, ns);
    fprintf('Longitude: %g° %s\n', longitude/1000000, ew);
end
