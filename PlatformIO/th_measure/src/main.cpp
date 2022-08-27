#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "sma.h"

WiFiClientSecure wifiClient;

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// const char* ssid = "TP-LINK_M5250_30228B";
// const char* password = "21275721";
const char *ssid = "DHARMA_NET";
const char *password = "JuanValdes5";
// const char* ssid = "kovi-deco";
// const char* password = "ga1Jione";
const char *sensor_url = "https://psaghelyi.ddns.net:12345/sensor/s4";

unsigned long startMillis = millis(); // some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long samples = 20;
const unsigned long halfMinute = 10 * 1000 / samples;

WiFiEventHandler gotIpEventHandler, disconnectedEventHandler;

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);

    // pinMode(LED_BUILTIN, OUTPUT);

    dht.begin();

    gotIpEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &event)
    {
        Serial.print("Station connected, IP: ");
        Serial.println(WiFi.localIP());
        Serial.flush();

        wifiClient.setInsecure(); 
    });

    disconnectedEventHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected &event)
    { 
        Serial.println("Station disconnected"); 
    });

    Serial.printf("Connecting to %s ...\n", ssid);
    WiFi.begin(ssid, password);
    WiFi.waitForConnectResult();
}

void upload_sensor(String const &st, String const &sh)
{
    String body = "{\"temperature\": " + st + ", \"humidity\": " + sh + "}";
    Serial.print(body);
    Serial.print(" --> ");

    HTTPClient http;
    http.begin(wifiClient, sensor_url);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.PUT(body);
    http.end();

    Serial.println(httpResponseCode);
    Serial.flush();
}

void loop()
{
    static SMA<samples> sma_tmp (dht.readTemperature());
    static SMA<samples> sma_hmd (dht.readHumidity());

    if (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        return;
    }
    
    currentMillis = millis();
    if (currentMillis - startMillis >= halfMinute)
    {        
        //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        //delay(250);
        //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

        float t = sma_tmp(dht.readTemperature());
        float h = sma_hmd(dht.readHumidity());

        static int counter;
        if (++counter == samples)
        {
            String st = isnan(t) ? String("") : String(t);
            String sh = isnan(h) ? String("") : String(h);
            upload_sensor(st, sh);
            counter = 0;
        }

        startMillis = currentMillis;
    }
}