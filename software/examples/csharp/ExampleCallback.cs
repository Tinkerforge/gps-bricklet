using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "ABC"; // Change to your UID

	// Callback function for coordinates callback
	static void CoordinatesCB(object sender,
	                          uint latitude, char ns, uint longitude, char ew, 
	                          ushort pdop, ushort hdop, ushort vdop, ushort epe)
	{

		System.Console.WriteLine("Latitude: " + latitude/1000000.0 + "° " + ns);
		System.Console.WriteLine("Longitude: " + longitude/1000000.0 + "° " + ew);
	}

	static void Main() 
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletGPS gps = new BrickletGPS(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Set Period for coordinates callback to 1s (1000ms)
		// Note: The current callback is only called every second if the 
		//       coordinates have changed since the last call!
		gps.SetCoordinatesCallbackPeriod(1000);

		// Register coordinates callback to function CoordinatesCB
		gps.Coordinates += CoordinatesCB;

		System.Console.WriteLine("Press key to exit");
		System.Console.ReadKey();
    }
}
