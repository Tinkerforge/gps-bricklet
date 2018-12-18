package main

import (
	"fmt"
	"github.com/Tinkerforge/go-api-bindings/gps_bricklet"
	"github.com/Tinkerforge/go-api-bindings/ipconnection"
)

const ADDR string = "localhost:4223"
const UID string = "XYZ" // Change XYZ to the UID of your GPS Bricklet.

func main() {
	ipcon := ipconnection.New()
	defer ipcon.Close()
	gps, _ := gps_bricklet.New(UID, &ipcon) // Create device object.

	ipcon.Connect(ADDR) // Connect to brickd.
	defer ipcon.Disconnect()
	// Don't use device before ipcon is connected.

	gps.RegisterCoordinatesCallback(func(latitude uint32, ns rune, longitude uint32, ew rune, pdop uint16, hdop uint16, vdop uint16, epe uint16) {
		fmt.Printf("Latitude: %f °\n", float64(latitude)/1000000.0)
		fmt.Printf("N/S: %c\n", ns)
		fmt.Printf("Longitude: %f °\n", float64(longitude)/1000000.0)
		fmt.Printf("E/W: %c\n", ew)
		fmt.Println()
	})

	// Set period for coordinates receiver to 1s (1000ms).
	// Note: The coordinates callback is only called every second
	//       if the coordinates has changed since the last call!
	gps.SetCoordinatesCallbackPeriod(1000)

	fmt.Print("Press enter to exit.")
	fmt.Scanln()

}
