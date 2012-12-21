import com.tinkerforge.BrickletGPS;
import com.tinkerforge.IPConnection;

public class ExampleSimple {
	private static final String host = "localhost";
	private static final int port = 4223;
	private static final String UID = "ABC"; // Change to your UID
	
	// Note: To make the example code cleaner we do not handle exceptions. Exceptions you
	//       might normally want to catch are described in the commnents below
	public static void main(String args[]) throws Exception {
		// Create connection to brickd
		IPConnection ipcon = new IPConnection(host, port); // Can throw IOException
		BrickletGPS gps = new BrickletGPS(UID); // Create device object

		// Add device to IP connection
		ipcon.addDevice(gps); // Can throw IPConnection.TimeoutException
		// Don't use device before it is added to a connection

		// Get GPS coordinates
		BrickletGPS.Coordinates coords = gps.getCoordinates(); // Can throw IPConnection.TimeoutException

		System.out.println("Latitude: " + coords.latitude/1000000.0 + "° " + coords.ns);
		System.out.println("Longitude: " + coords.longitude/1000000.0 + "° " + coords.ew);

		System.console().readLine("Press key to exit\n");
		ipcon.destroy();
	}
}
