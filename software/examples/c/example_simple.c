#include <stdio.h>

#include "ip_connection.h"
#include "bricklet_gps.h"

#define HOST "localhost"
#define PORT 4223
#define UID "XYZ" // Change to your UID

int main(void) {
	// Create IP connection
	IPConnection ipcon;
	ipcon_create(&ipcon);

	// Create device object
	GPS gps;
	gps_create(&gps, UID, &ipcon);

	// Connect to brickd
	if(ipcon_connect(&ipcon, HOST, PORT) < 0) {
		fprintf(stderr, "Could not connect\n");
		return 1;
	}
	// Don't use device before ipcon is connected

	// Get current coordinates
	uint32_t latitude, longitude;
	char ns, ew;
	uint16_t pdop, hdop, vdop, epe;
	if(gps_get_coordinates(&gps, &latitude, &ns, &longitude, &ew,
	                       &pdop, &hdop, &vdop, &epe) < 0) {
		fprintf(stderr, "Could not get coordinates, probably timeout\n");
		return 1;
	}

	printf("Latitude: %f° %c\n", latitude/1000000.0, ns);
	printf("Longiutde: %f° %c\n", longitude/1000000.0, ew);

	printf("Press key to exit\n");
	getchar();
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
	return 0;
}
