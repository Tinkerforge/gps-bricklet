import com.tinkerforge.BrickletGPS;
import com.tinkerforge.IPConnection;

public class ExampleCallback {
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

		// Set Period for coordinates callback to 1s (1000ms)
		// Note: The current callback is only called every second if the 
		//       coordinates have changed since the last call!
		gps.setCoordinatesCallbackPeriod(1000);

		// Add and implement coordinates listener (called if coordinates change)
		gps.addListener(new BrickletGPS.CoordinatesListener() {
			public void coordinates(long latitude, char ns, long longitude, char ew,
			                        int pdop, int hdop, int vdop, int epe) {
				System.out.println("Latitude: " + latitude/1000000.0 + "° " + ns);
				System.out.println("Longitude: " + longitude/1000000.0 + "° " + ew);
			}
		});

		System.console().readLine("Press key to exit\n");
		ipcon.destroy();
	}
}
