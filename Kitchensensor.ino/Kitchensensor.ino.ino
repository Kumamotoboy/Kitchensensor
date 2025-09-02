#include "DHT.h"
#include <WiFi.h>

// Pin to which the DHT11 sensor data line is connected
#define DHTPIN 4
// Define the sensor type as DHT11
#define DHTTYPE DHT11

// WiFi credentials
const char* ssid = "YOUR SSID";
const char* password = "YOUR PASSWORD";

// Create a WiFi server on port 80 (HTTP)
WiFiServer server(80);

// Initialize the DHT sensor
DHT dht(DHTPIN, DHTTYPE);
 
void setup() {
  // Start serial communication for debugging at 115200 baud
  Serial.begin(115200);
  
  // Connect to the WiFi network using defined credentials
  WiFi.begin(ssid, password);
  // Wait until the WiFi is connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Print connection successful message and IP address
  Serial.println("\nWiFi connected.");
  Serial.print("IP-address: ");
  Serial.println(WiFi.localIP());

  // Start the HTTP server
  server.begin();

  // Initialize the DHT sensor
  dht.begin();
}
 
void loop() {
  // Check if a new client has connected
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("New client connected.");
    String currentLine = "";
    String request = "";

    // While the client is connected, read incoming data
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;

        // If a newline character is received, parse the HTTP header lines
        if (c == '\n') {
          // An empty line indicates the end of the HTTP header section
          if (currentLine.length() == 0) {
            // Check which URL endpoint was requested
            if (request.indexOf("GET /api/temperature") >= 0) {
              // Read temperature from DHT sensor
              float t = dht.readTemperature();

              // Send HTTP 200 response with temperature data
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/plain");
              client.println("Connection: close");
              client.println();
              client.print(String(t) + "C");
            } 
            else if (request.indexOf("GET /api/humidity") >= 0) {
              Read humidity from DHT sensor
              float h = dht.readHumidity();

              // Send HTTP 200 response with humidity data
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/plain");
              client.println("Connection: close");
              client.println();
              client.print(String(h) + "%");
            } 
            else {
              // For unrecognized endpoints, send HTTP 404 response
              client.println("HTTP/1.1 404 Not Found");
              client.println("Content-Type: text/plain");
              client.println("Connection: close");
              client.println();
              client.print("Nicht gefunden");
            }
            break; // Exit the HTTP request processing loop
          } else {
            // Clear the line buffer to read the next header line
            currentLine = "";
          }
        // Ignore carriage return characters
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    delay(1);

    // Close the client connection
    client.stop();
    Serial.println("Client disconnected.");
  }
}
