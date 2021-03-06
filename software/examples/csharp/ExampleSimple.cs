using System;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change XYZ to the UID of your GPS Bricklet

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletGPS gps = new BrickletGPS(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Get current coordinates
		long latitude, longitude; char ns, ew; int pdop, hdop, vdop, epe;
		gps.GetCoordinates(out latitude, out ns, out longitude, out ew, out pdop,
		                   out hdop, out vdop, out epe);

		Console.WriteLine("Latitude: " + latitude/1000000.0 + " °");
		Console.WriteLine("N/S: " + ns);
		Console.WriteLine("Longitude: " + longitude/1000000.0 + " °");
		Console.WriteLine("E/W: " + ew);

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		ipcon.Disconnect();
	}
}
