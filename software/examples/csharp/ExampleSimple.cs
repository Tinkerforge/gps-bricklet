using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "ABC"; // Change to your UID

	static void Main() 
	{
		IPConnection ipcon = new IPConnection(HOST, PORT); // Create connection to brickd
		BrickletGPS gps = new BrickletGPS(UID); // Create device object
		ipcon.AddDevice(gps); // Add device to ip connection
		// Don't use device before it is added to a connection


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

		System.Console.WriteLine("Press ctrl+c to exit");
		ipcon.JoinThread();
    }
}
