#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WebServer.h>

#include "config.h"
#include "backend.h"
#include "sen55.h"

#define HOST_NAME "ESP32-" SENSOR_NAME
#define VERSION "2023.02.25"

Backend     backend(HOST_NAME);
Sen55       sen55;
WebServer   webServer(80);

unsigned long       startSensorMillis = millis();
unsigned long       currentMillis;
const unsigned long samples = 10;
const unsigned long rate = 10 * 60 * 1000 / samples;  // 10 minutes - 10 samples

void handleRoot() {
  webServer.send(200, "application/json; charset=UTF-8", sen55.read());
}

void handleVersion() {
  webServer.send(200, "text/plain", VERSION);
}

void handleNotFound(){
  webServer.send(404, "text/plain", "404: Not found");
}

void setup() 
{
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

    backend.connect_wifi();
    sen55.init();

    
    ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

    ArduinoOTA.setHostname(HOST_NAME);
    ArduinoOTA.begin();

    MDNS.addService("http", "tcp", 80);
    MDNS.begin(HOST_NAME);

    webServer.on("/", HTTP_GET, handleRoot);
    webServer.on("/version", HTTP_GET, handleVersion);
    webServer.onNotFound(handleNotFound);
    webServer.begin();
}


void loop() 
{
    ArduinoOTA.handle();
    webServer.handleClient();
    
    currentMillis = millis();
        if (currentMillis - startSensorMillis < rate)
    return;

    String data = sen55.read();
    backend.upload(data.c_str());

    startSensorMillis = currentMillis;
}
