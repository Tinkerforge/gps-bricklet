#include <stdio.h>

#include "ip_connection.h"
#include "bricklet_gps.h"

#define HOST "localhost"
#define PORT 4223
#define UID "ABC" // Change to your UID

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

	// Get current coordinates
	uint32_t ret_latitude, ret_longitude;
	char ret_ns, ret_ew;
	uint16_t ret_pdop, ret_hdop, ret_vdop, ret_epe;
	if(gps_get_coordinates(&gps, 
	                       &ret_latitude, &ret_ns, &ret_longitude, &ret_ew, 
	                       &ret_pdop, &ret_hdop, &ret_vdop, &ret_epe) < 0) {
		fprintf(stderr, "Could not get value, probably timeout\n");
		exit(1);
	}

	printf("Latitude: %f° %c\n", ret_latitude/1000000.0, ret_ns);
	printf("Longiutde: %f° %c\n", ret_longitude/1000000.0, ret_ew);

	printf("Press key to exit\n");
	getchar();
	ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
}
