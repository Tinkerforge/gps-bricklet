
#include <stdio.h>

#include "ip_connection.h"
#include "bricklet_gps.h"

#define HOST "localhost"
#define PORT 4223
#define UID "ABC" // Change to your UID

// Callback function for current callback (parameter has unit mA)
void cb_coordinates(uint32_t latitude, char ns, uint32_t longitude, char ew,
                    uint16_t pdop, uint16_t hdop, uint16_t vdop, uint16_t epe) {
	printf("Latitude: %f ° %c\n", latitude/1000000.0, ns);
	printf("Longiutde: %f ° %c\n", longitude/1000000.0, ew);
}

int main() {
	// Create ip connection to brickd
	IPConnection ipcon;
	if(ipcon_create(&ipcon, HOST, PORT) < 0) {
		fprintf(stderr, "Could not create connection\n");
		exit(1);
	}

	// Create device object
	GPS gps;
	gps_create(&gps, UID); 

	// Add device to ip connection
	if(ipcon_add_device(&ipcon, &gps) < 0) {
		fprintf(stderr, "Could not connect to Brick\n");
		exit(1);
	}
	// Don't use device before it is added to a connection

	// Set Period for coordinates callback to 1s (1000ms)
	// Note: The callback is only called every second if the 
	//       coordinates have changed since the last call!
	gps_set_coordinates_callback_period(&gps, 1000);

	// Register coordinates callback to function cb_coordinates
	gps_register_callback(&gps, GPS_CALLBACK_COORDINATES, cb_coordinates);

	printf("Press ctrl+c to close\n");
	ipcon_join_thread(&ipcon); // Join mainloop of ip connection
}
