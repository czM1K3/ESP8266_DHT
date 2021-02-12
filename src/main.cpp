#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include "Configuration.h"

const char* ssid = SSID;
const char* password = PASSWORD;

ESP8266WebServer server(80);

DHT dht(DHT_PIN, DHT11);

void sendData();

void setup() {
#ifdef SERIAL_BAUD
  Serial.begin(SERIAL_BAUD);

  Serial.println("Connecting to WiFi");
#endif
  WiFi.begin(ssid, password);
  WiFi.config(SATIC_IP, GATEWAY, SUBNET);
  while (!WiFi.isConnected())
  {
#ifdef SERIAL_BAUD
    Serial.print(".");
#endif
    delay(100);
  }
#ifdef SERIAL_BAUD
  Serial.println("WiFi connected");
#endif

  server.on("/", sendData);
  server.begin();
  dht.begin();
}

void loop() {
  server.handleClient();
}

void sendData() {
  StaticJsonDocument<200> doc;
  doc["temp"] = dht.readTemperature();
  doc["humidity"] = dht.readHumidity();

  String output;
  serializeJson(doc, output);
  server.send(200, "application/json", output);
}