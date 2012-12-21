program ExampleCallback;

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
    procedure CoordinatesCB(sender: TObject;
	                        const latitude: longword;
                            const ns: char;
                            const longitude: longword;
                            const ew: char;
                            const pdop: word;
                            const hdop: word;
                            const vdop: word;
                            const epe: word);
    procedure Execute;
  end;

const
  HOST = 'localhost';
  PORT = 4223;
  UID = 'ABC'; { Change to your UID }

var
  e: TExample;

{ Callback function for coordinates callback }
procedure TExample.CoordinatesCB(
sender: TObject;
const latitude: longword;
const ns: char;
const longitude: longword;
const ew: char;
const pdop: word;
const hdop: word;
const vdop: word;
const epe: word
);
begin
  WriteLn(Format('Latitude: %f ° %c', [latitude/1000000.0, ns]));
  WriteLn(Format('Longitude: %f ° %c', [longitude/1000000.0, ew]));
end;

procedure TExample.Execute;
begin
  { Create IP connection }
  ipcon := TIPConnection.Create;

  { Create device object }
  gps := TBrickletGPS.Create(UID, ipcon);

  { Connect to brickd }
  ipcon.Connect(HOST, PORT);
  { Don't use device before ipcon is connected }


  { Set Period for coordinates callback to 1s (1000ms)
    Note: The current callback is only called every second if the
          coordinates have changed since the last call! }
  gps.SetCoordinatesCallbackPeriod(1000);

  { Register coordinates callback to procedure CoordinatesCB }
  gps.OnCoordinates := {$ifdef FPC}@{$endif}CoordinatesCB;

  WriteLn('Press key to exit');
  ReadLn;
  ipcon.Destroy;
end;

begin
  e := TExample.Create;
  e.Execute;
  e.Destroy;
end.
