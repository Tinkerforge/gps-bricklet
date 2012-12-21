using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "ABC"; // Change to your UID

	static void Main() 
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletGPS gps = new BrickletGPS(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		uint latitude;
		char ns;
		uint longitude;
		char ew;
		ushort pdop, hdop, vdop, epe;
		// Get current current and voltage (unit is mA and mV)
		gps.GetCoordinates(out latitude, out ns, out longitude, out ew, 
		                   out pdop, out hdop, out vdop, out epe);

		System.Console.WriteLine("Latitude: " + latitude/1000000.0 + "° " + ns);
		System.Console.WriteLine("Longitude: " + longitude/1000000.0 + "° " + ew);

		System.Console.WriteLine("Press key to exit");
		System.Console.ReadKey();
    }
}
