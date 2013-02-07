
#include <stdio.h>

#include "ip_connection.h"
#include "bricklet_gps.h"

#define HOST "localhost"
#define PORT 4223
#define UID "ABC" // Change to your UID

// Callback function for current callback (parameter has unit mA)
void cb_coordinates(uint32_t latitude, char ns, uint32_t longitude, char ew,
                    uint16_t pdop, uint16_t hdop, uint16_t vdop, uint16_t epe, 
                    void *user_data) {
	(void)pdop; (void)hdop; (void)vdop; (void)epe;
	(void)user_data;

	printf("Latitude: %f° %c\n", latitude/1000000.0, ns);
	printf("Longiutde: %f° %c\n", longitude/1000000.0, ew);
}

int main() {
	// Create ip connection to brickd
	IPConnection ipcon;
	ipcon_create(&ipcon);

	// Create device object
	GPS gps;
	gps_create(&gps, UID, &ipcon); 

	// Connect to brickd
	if(ipcon_connect(&ipcon, HOST, PORT) < 0) {
		fprintf(stderr, "Could not connect\n");
		exit(1);
	}
	// Don't use device before ipcon is connected

	// Set Period for coordinates callback to 1s (1000ms)
	// Note: The callback is only called every second if the 
	//       coordinates have changed since the last call!
	gps_set_coordinates_callback_period(&gps, 1000);

	// Register coordinates callback to function cb_coordinates
	gps_register_callback(&gps, GPS_CALLBACK_COORDINATES,
	                      (void *)cb_coordinates, NULL);

	printf("Press key to exit\n");
	getchar();
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
}
