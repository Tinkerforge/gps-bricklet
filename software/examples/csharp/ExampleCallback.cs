using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "ABC"; // Change to your UID

	// Callback function for coordinates callback
	static void CoordinatesCB(uint latitude, char ns, uint longitude, char ew, 
	                          ushort pdop, ushort hdop, ushort vdop, ushort epe)
	{

		System.Console.WriteLine("Latitude: " + latitude/1000000.0 + "° " + ns);
		System.Console.WriteLine("Longitude: " + longitude/1000000.0 + "° " + ew);
	}

	static void Main() 
	{
		IPConnection ipcon = new IPConnection(HOST, PORT); // Create connection to brickd
		BrickletGPS gps = new BrickletGPS(UID); // Create device object
		ipcon.AddDevice(gps); // Add device to ip connection
		// Don't use device before it is added to a connection


		// Set Period for coordinates callback to 1s (1000ms)
		// Note: The current callback is only called every second if the 
		//       coordinates have changed since the last call!
		gps.SetCoordinatesCallbackPeriod(1000);

		// Register coordinates callback to function CoordinatesCB
		gps.RegisterCallback(new BrickletGPS.Coordinates(CoordinatesCB));

		System.Console.WriteLine("Press ctrl+c to exit");
		ipcon.JoinThread();
    }
}
