using System;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change to your UID

	// Callback function for coordinates callback
	static void CoordinatesCB(BrickletGPS sender,
	                          long latitude, char ns, long longitude, char ew, 
	                          int pdop, int hdop, int vdop, int epe)
	{
		Console.WriteLine("Latitude: " + latitude/1000000.0 + "° " + ns);
		Console.WriteLine("Longitude: " + longitude/1000000.0 + "° " + ew);
		Console.WriteLine("");
	}

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletGPS gps = new BrickletGPS(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Register coordinates callback to function CoordinatesCB
		gps.Coordinates += CoordinatesCB;

		// Set period for coordinates callback to 1s (1000ms)
		// Note: The coordinates callback is only called every second
		//       if the coordinates has changed since the last call!
		gps.SetCoordinatesCallbackPeriod(1000);

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		ipcon.Disconnect();
	}
}
