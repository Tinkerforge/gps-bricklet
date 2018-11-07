use std::{error::Error, io};

use tinkerforge::{gps_bricklet::*, ip_connection::IpConnection};

const HOST: &str = "localhost";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your GPS Bricklet.

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection.
    let gps = GpsBricklet::new(UID, &ipcon); // Create device object.

    ipcon.connect((HOST, PORT)).recv()??; // Connect to brickd.
                                          // Don't use device before ipcon is connected.

    // Get current coordinates.
    let get_coordinates_result = gps.get_coordinates().recv()?;

    println!("Latitude: {} °", get_coordinates_result.latitude as f32 / 1000000.0);
    println!("N/S: {}", get_coordinates_result.ns);
    println!("Longitude: {} °", get_coordinates_result.longitude as f32 / 1000000.0);
    println!("E/W: {}", get_coordinates_result.ew);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
