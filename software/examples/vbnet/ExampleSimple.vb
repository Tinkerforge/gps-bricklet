Imports Tinkerforge

Module ExampleSimple
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change to your UID

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim gps As New BrickletGPS(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        Dim latitude, longitude As Long
        Dim ns, ew As Char
        Dim pdop, hdop, vdop, epe As Integer

        ' Get current coordinates
        gps.GetCoordinates(latitude, ns, longitude, ew, pdop, hdop, vdop, epe)

        System.Console.WriteLine("Latitude: " + (latitude/1000000.0).ToString() + "° " + ns)
        System.Console.WriteLine("Longitude: " + (longitude/1000000.0).ToString() + "° " + ew)

        System.Console.WriteLine("Press key to exit")
        System.Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
