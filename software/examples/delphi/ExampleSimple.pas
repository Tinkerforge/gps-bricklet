program ExampleSimple;

{$ifdef MSWINDOWS}{$apptype CONSOLE}{$endif}
{$ifdef FPC}{$mode OBJFPC}{$H+}{$endif}

uses
  SysUtils, IPConnection, BrickletGPS;

type
  TExample = class
  private
    ipcon: TIPConnection;
    gps: TBrickletGPS;
  public
    procedure Execute;
  end;

const
  HOST = 'localhost';
  PORT = 4223;
  UID = 'ABC'; { Change to your UID }

var
  e: TExample;

procedure TExample.Execute;
var latitude: longword;
var ns: char;
var longitude: longword;
var ew: char;
var pdop: word;
var hdop: word;
var vdop: word;
var epe: word;
begin
  { Create IP connection to brickd }
  ipcon := TIPConnection.Create(HOST, PORT);

  { Create device object }
  gps := TBrickletGPS.Create(UID);

  { Add device to IP connection }
  ipcon.AddDevice(gps);
  { Don't use device before it is added to a connection }

  { Get current coordinates }
  gps.GetCoordinates(latitude, ns, longitude, ew, pdop, hdop, vdop, epe);
  WriteLn(Format('Latitude: %f ° %c', [latitude/1000000.0, ns]));
  WriteLn(Format('Longitude: %f ° %c', [longitude/1000000.0, ew]));

  WriteLn('Press key to exit');
  ReadLn;
  ipcon.Destroy;
end;

begin
  e := TExample.Create;
  e.Execute;
  e.Destroy;
end.
