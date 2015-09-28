Imports System
Imports Tinkerforge

Module ExampleCallback
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change to your UID

    ' Callback subroutine for coordinates callback
    Sub CoordinatesCB(ByVal sender As BrickletGPS, _
                      ByVal latitude As Long, ByVal ns As Char, _
                      ByVal longitude As Long, ByVal ew As Char, _
                      ByVal pdop As Integer, ByVal hdop As Integer, ByVal vdop As Integer, _
                      ByVal epe As Integer)
        Console.WriteLine("Latitude: " + (latitude/1000000.0).ToString() + "° " + ns)
        Console.WriteLine("Longitude: " + (longitude/1000000.0).ToString() + "° " + ew)
        Console.WriteLine("")
    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim gps As New BrickletGPS(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Register coordinates callback to subroutine CoordinatesCB
        AddHandler gps.Coordinates, AddressOf CoordinatesCB

        ' Set period for coordinates callback to 1s (1000ms)
        ' Note: The coordinates callback is only called every second
        '       if the coordinates has changed since the last call!
        gps.SetCoordinatesCallbackPeriod(1000)

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
