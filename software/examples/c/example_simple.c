
#include <stdio.h>

#include "ip_connection.h"
#include "bricklet_gps.h"

#define HOST "localhost"
#define PORT 4223
#define UID "ABC" // Change to your UID

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

	// Get current coordinates
	uint32_t ret_latitude;
	char ret_ns;
	uint32_t ret_longitude;
	char ret_ew;
	uint16_t ret_pdop;
	uint16_t ret_hdop;
	uint16_t ret_vdop;
	uint16_t ret_epe;
	if(gps_get_coordinates(&gps, 
	                       &ret_latitude, &ret_ns, &ret_longitude, &ret_ew, 
	                       &ret_pdop, &ret_hdop, &ret_vdop, &ret_epe) < 0) {
		fprintf(stderr, "Could not get value, probably timeout\n");
		exit(1);
	}

	printf("Latitude: %f ° %c\n", ret_latitude/1000000.0, ret_ns);
	printf("Longiutde: %f ° %c\n", ret_longitude/1000000.0, ret_ew);

	printf("Press ctrl+c to close\n");
	ipcon_join_thread(&ipcon); // Join mainloop of ip connection
}
