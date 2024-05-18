# Sensor ESP32

Simple project to read sensors connected to a given PIN using `POST` and `GET` requests.

## Hardware requirements

- ESP32
- Bridge H L298
- Sensors

## Software Requirements

- [VSCode](https://code.visualstudio.com/)
- Extensions:
  - [PlatformIO](https://platformio.org/)
  - [C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack)
- Libraries:
  - ESPAsyncWebServer

## Endpoints

- `/engine/on`: Activates the engine.
- `/engine/off`: Deactivates the engine.
- `/engine/speed`: Adjusts the engine speed.
- `/sensor`: Read physical sensor.

This project was created by JMota during the C3 P1.