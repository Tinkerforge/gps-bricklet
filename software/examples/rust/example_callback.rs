use std::{error::Error, io, thread};
use tinkerforge::{gps_bricklet::*, ipconnection::IpConnection};

const HOST: &str = "127.0.0.1";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your GPS Bricklet

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection
    let gps_bricklet = GPSBricklet::new(UID, &ipcon); // Create device object

    ipcon.connect(HOST, PORT).recv()??; // Connect to brickd
                                        // Don't use device before ipcon is connected

    //Create listener for coordinates events.
    let coordinates_listener = gps_bricklet.get_coordinates_receiver();
    // Spawn thread to handle received events. This thread ends when the gps_bricklet
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for event in coordinates_listener {
            println!("Latitude: {}{}", event.latitude as f32 / 1000000.0, " °");
            println!("N/S: {}", event.ns);
            println!("Longitude: {}{}", event.longitude as f32 / 1000000.0, " °");
            println!("E/W: {}", event.ew);
            println!();
        }
    });

    // Set period for coordinates listener to 1s (1000ms)
    // Note: The coordinates callback is only called every second
    //       if the coordinates has changed since the last call!
    gps_bricklet.set_coordinates_callback_period(1000);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
