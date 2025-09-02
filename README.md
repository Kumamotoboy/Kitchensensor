# \# ESP32 DHT11 Sensor HTTP Interface

# 

# This project is an Arduino-style ESP32 program that reads temperature and humidity values from a DHT11 sensor connected to GPIO 4. It is designed to be integrated with automation tools like Streamer.Bot by enabling HTTP requests to trigger or retrieve sensor data.

# 

# \## Features

# 

# \- Reads temperature (°C) and humidity (%) from a DHT11 sensor.

# \- Outputs sensor readings to the serial console.

# \- Can be extended to provide HTTP endpoints for external applications such as Streamer.Bot.

# 

# \## Hardware Setup

# 

# \- ESP32 development board.

# \- DHT11 temperature and humidity sensor.

# \- Connect the sensor data pin to GPIO 4 of the ESP32.

# \- Provide power and ground connections to the sensor as per the sensor specification.

# 

# \## Code Overview

# 

# \- Uses the `DHT` library to interface with the sensor.

# \- Initializes the sensor on GPIO 4 with type DHT11.

# \- In the main loop, it reads humidity and temperature every 2 seconds, printing the values to the serial console.

# \- If the sensor data read fails, it outputs an error message.

# 

# 

