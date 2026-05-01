#include "DHTesp.h" // https://github.com/beegee-tokyo/DHTesp
#include <WiFi.h>

#define DHTPIN 4

const char* ssid = "YOUR SSID";
const char* password = "YOUR PASSWORD";

WiFiServer server(80);
DHTesp dht;

float lastTemperatureC = NAN;
float lastHumidityPct = NAN;
unsigned long lastSensorReadMs = 0;
const unsigned long SENSOR_READ_INTERVAL_MS = 2000;

void sendText(WiFiClient &client, int code, const char* body) {
  const char* status = (code==200 ? "OK" : (code==404 ? "Not Found" : "Service Unavailable"));
  client.print("HTTP/1.1 ");
  client.print(code);
  client.print(" ");
  client.print(status);
  client.print("\r\n");
  client.println("Content-Type: text/plain; charset=utf-8");
  client.println("Connection: close");
  client.println();
  client.print(body);
}

void updateSensorCache() {
  TempAndHumidity reading = dht.getTempAndHumidity();

  if (!isnan(reading.temperature)) {
    lastTemperatureC = reading.temperature;
  }

  if (!isnan(reading.humidity)) {
    lastHumidityPct = reading.humidity;
  }

  lastSensorReadMs = millis();
}

String normalizePath(String path) {
  int queryIndex = path.indexOf('?');
  if (queryIndex >= 0) {
    path = path.substring(0, queryIndex);
  }

  while (path.length() > 1 && path.endsWith("/")) {
    path.remove(path.length() - 1);
  }

  return path;
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) { // kein endlos loop [8](https://forum.arduino.cc/t/implement-wifi-connecting-timeout-on-esp32/645718)
    delay(250);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("WiFi status: "); Serial.println(WiFi.status());
  Serial.print("IP: "); Serial.println(WiFi.localIP());

  server.begin();
  dht.setup(DHTPIN, DHTesp::DHT11);
  updateSensorCache();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
    delay(100);
    return;
  }

  if (millis() - lastSensorReadMs >= SENSOR_READ_INTERVAL_MS) {
    updateSensorCache();
  }

  WiFiClient client = server.available();
  if (!client) return;

  client.setTimeout(2000);

  // Nur Request-Line lesen (statt Request zusammensetzen) -> weniger Fragmentierung [1](https://hubble.com/community/guides/esp32-memory-fragmentation-why-your-device-crashes-after-running-for-days/)[2](https://forum.arduino.cc/t/esp32-memory-questions-heap-frag-related/1343460)
  String line = client.readStringUntil('\r');
  client.readStringUntil('\n');

  if (!line.startsWith("GET ")) {
    client.stop();
    return;
  }

  int firstSpace = line.indexOf(' ');
  int secondSpace = line.indexOf(' ', firstSpace + 1);
  String path = (firstSpace >= 0 && secondSpace > firstSpace)
    ? line.substring(firstSpace + 1, secondSpace)
    : "";
  path = normalizePath(path);

  if (path.length() == 0) {
    client.stop();
    return;
  }

  if (path.startsWith("/api/temperature")) {
    if (isnan(lastTemperatureC)) { sendText(client, 503, "sensor error"); }
    else {
      String out = String(lastTemperatureC, 1) + "C";
      sendText(client, 200, out.c_str());
    }
  }
  else if (path.startsWith("/api/humidity")) {
    if (isnan(lastHumidityPct)) { sendText(client, 503, "sensor error"); }
    else {
      String out = String(lastHumidityPct, 1) + "%";
      sendText(client, 200, out.c_str());
    }
  }
  else {
    sendText(client, 404, "Nicht gefunden");
  }

  client.stop();
}