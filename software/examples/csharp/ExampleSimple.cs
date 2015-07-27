using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change to your UID

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletGPS gps = new BrickletGPS(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		long latitude, longitude;
		char ns, ew;
		int pdop, hdop, vdop, epe;

		// Get current coordinates
		gps.GetCoordinates(out latitude, out ns, out longitude, out ew, 
		                   out pdop, out hdop, out vdop, out epe);

		System.Console.WriteLine("Latitude: " + latitude/1000000.0 + "° " + ns);
		System.Console.WriteLine("Longitude: " + longitude/1000000.0 + "° " + ew);

		System.Console.WriteLine("Press enter to exit");
		System.Console.ReadLine();
		ipcon.Disconnect();
	}
}
