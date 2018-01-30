Imports System
Imports Tinkerforge

Module ExampleSimple
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your GPS Bricklet

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim gps As New BrickletGPS(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Get current coordinates
        Dim latitude, longitude As Long
        Dim ns, ew As Char
        Dim pdop, hdop, vdop, epe As Integer

        gps.GetCoordinates(latitude, ns, longitude, ew, pdop, hdop, vdop, epe)

        Console.WriteLine("Latitude: " + (latitude/1000000.0).ToString() + " °")
        Console.WriteLine("N/S: " + ns)
        Console.WriteLine("Longitude: " + (longitude/1000000.0).ToString() + " °")
        Console.WriteLine("E/W: " + ew)

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
