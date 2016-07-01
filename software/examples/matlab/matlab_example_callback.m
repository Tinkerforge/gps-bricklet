function matlab_example_callback()
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletGPS;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change XYZ to the UID of your GPS Bricklet

    ipcon = IPConnection(); % Create IP connection
    gps = handle(BrickletGPS(UID, ipcon), 'CallbackProperties'); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register coordinates callback to function cb_coordinates
    set(gps, 'CoordinatesCallback', @(h, e) cb_coordinates(e));

    % Set period for coordinates callback to 1s (1000ms)
    % Note: The coordinates callback is only called every second
    %       if the coordinates has changed since the last call!
    gps.setCoordinatesCallbackPeriod(1000);

    input('Press key to exit\n', 's');
    ipcon.disconnect();
end

% Callback function for coordinates callback
function cb_coordinates(e)
    fprintf('Latitude: %g° %s\n', e.latitude/1000000.0, e.ns);
    fprintf('Longitude: %g° %s\n', e.longitude/1000000.0, e.ew);
    fprintf('\n');
end
