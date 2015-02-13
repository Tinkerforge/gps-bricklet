Imports Tinkerforge

Module ExampleCallback
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "cYj" ' Change to your UID

    ' Callback function for coordinates callback
    Sub CoordinatesCB(ByVal sender As BrickletGPS, _
                      ByVal latitude As Long, ByVal ns As Char, _
                      ByVal longitude As Long, ByVal ew As Char, _
                      ByVal pdop As Integer, ByVal hdop As Integer, ByVal vdop As Integer, _
                      ByVal epe As Integer)
        System.Console.WriteLine("Latitude: " + (latitude/1000000.0).ToString() + "° " + ns)
        System.Console.WriteLine("Longitude: " + (longitude/1000000.0).ToString() + "° " + ew)
    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim gps As New BrickletGPS(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Set Period for coordinates callback to 1s (1000ms)
        ' Note: The current callback is only called every second if the
        '       coordinates have changed since the last call!
        gps.SetCoordinatesCallbackPeriod(1000)

        ' Register coordinates callback to function CoordinatesCB
        AddHandler gps.Coordinates, AddressOf CoordinatesCB

        System.Console.WriteLine("Press key to exit")
        System.Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
