#include <Arduino.h>
#include <ArduinoOTA.h>
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

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {
      type = "filesystem";
    }
#ifdef SERIAL_BAUD
    Serial.println("Start updating " + type);
#endif
  });
  ArduinoOTA.onEnd([]() {
#ifdef SERIAL_BAUD
    Serial.println("\nEnd");
#endif
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
#ifdef SERIAL_BAUD
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
#endif
  });
  ArduinoOTA.onError([](ota_error_t error) {
#ifdef SERIAL_BAUD
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
#endif
  });
  ArduinoOTA.begin();
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
}

void sendData() {
  StaticJsonDocument<200> doc;
  doc["temp"] = dht.readTemperature();
  doc["humidity"] = dht.readHumidity();

  String output;
  serializeJson(doc, output);
  server.send(200, "application/json", output);
}