# ESP32 DHT11 Sensor HTTP Interface

This project is an Arduino-style ESP32 program that reads temperature and humidity values from a DHT11 sensor connected to GPIO 4 and exposes them via HTTP endpoints. It is designed for integration with automation tools like Streamer.Bot.

## Features 

- Reads temperature (°C) and humidity (%) from a DHT11 sensor every 2 seconds.
- Provides HTTP API endpoints for retrieving current sensor readings.
- Caches the last valid reading to ensure stable responses even if a sensor read fails.
- Outputs connection status to the serial console.
- Normalizes browser requests (handles query strings, trailing slashes, favicon requests).

## Hardware Setup

- ESP32 development board.
- DHT11 temperature and humidity sensor.
- Connect the sensor data pin to GPIO 4 of the ESP32.
- Provide power and ground connections to the sensor as per the sensor specification.

## Library

- Uses the [DHT sensor library for ESPx](https://github.com/beegee-tokyo/DHTesp) by beegee-tokyo.
- Install via Arduino Library Manager: search for `DHTesp`.

## HTTP API Endpoints

| Endpoint | Method | Response |
|----------|--------|----------|
| `/api/temperature` | GET | Returns the last valid temperature reading in °C (e.g., `23.5C`) |
| `/api/humidity` | GET | Returns the last valid humidity reading in % (e.g., `45.2%`) |

**Example:**
```
http://192.168.178.65/api/temperature
http://192.168.178.65/api/humidity
```

## Configuration

Edit the following constants in `Kitchensensor.ino`:

```cpp
const char* ssid = "YOUR SSID";           // WiFi network name
const char* password = "YOUR PASSWORD";   // WiFi password
#define DHTPIN 4                          // GPIO pin for DHT sensor (default: GPIO 4)
```

## Code Overview

- Initializes WiFi with a 15-second timeout.
- Sets up the DHT11 sensor on GPIO 4.
- Reads the sensor every 2000 ms (`SENSOR_READ_INTERVAL_MS`).
- Caches the last valid temperature and humidity values.
- Serves HTTP requests on port 80 with normalized path parsing.
- Returns 200 (OK) with the reading, 503 (Service Unavailable) if no valid reading has been cached yet, or ignores malformed requests.
