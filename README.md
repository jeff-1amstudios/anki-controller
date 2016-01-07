### Anki Drive SDK (Native OSX CoreBluebooth)

[anki-sdk](https://github.com/anki/drive-sdk) says it compiles cleanly on OSX, but it doesn't.

I took the anki-sdk C files which generate the data packets, and wrapped them up in OSX CoreBluetooth code for the discovery and communication with the cars.

This project builds a console app, which allows you to control multiple cars via standard input.  Commands are expected in JSON format:

```
{ "command": "scan" } -> starts scanning for bluetooth devices
{ "command": "stop-scan" } -> stops scanning for bluebooth devices
{ "command": "list-cars" } -> outputs a list of discovered cars
{ "command": "set-speed", "carId": "<id>", "value": 500 } -> sets speed of specified car
{ "command": "spin", "carId": "<id>" } -> specified car does a 180 degree spin
```
