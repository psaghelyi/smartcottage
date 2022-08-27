#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include "sma.h"

#define SENSOR_NAME "mk4"
#define OTA_Host_Name "ESP01-"SENSOR_NAME

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

WiFiClientSecure wifiClient;

//const char* ssid = "TP-LINK_M5250_30228B";
//const char* pass = "21275721";
//const char* ssid = "DHARMA_NET";
//const char* pass = "JuanValdes5";
//const char* ssid = "kovi-deco";
//const char* pass = "ga1Jione";
const char* sensor_url = "https://psaghelyi.ddns.net:12345/sensor/"SENSOR_NAME;


unsigned long startMillis = millis();
unsigned long currentMillis;
const unsigned long samples = 10;
const unsigned long rate = 10 * 60 * 1000 / samples;  // 10 minutes - 10 samples


void connect_wifi() {
  if (WiFi.status() == WL_CONNECTED)
    return;

  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.flush();

  wifiClient.setInsecure();
}

void disconnect_wifi() {
  WiFi.disconnect();
  Serial.println("Wifi disconnected");
  Serial.flush();
}

void upload_sensor(String const & st, String const & sh)
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

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  // dht needs 2 seconds to settle
  delay(2000);

  connect_wifi();

  ArduinoOTA.setHostname(OTA_Host_Name);
  ArduinoOTA.begin();
}

void loop()
{
  ArduinoOTA.handle();
  
  static SMA<samples * 2> sma_tmp;
  static SMA<samples * 2> sma_hmd;
  static bool init;
  if (!init)
  {
    sma_tmp.reset(dht.readTemperature() * 10.);
    sma_hmd.reset(dht.readHumidity() * 10.);
    init = true;
  }

  currentMillis = millis();
  if (currentMillis - startMillis >= rate)
  {        
    float t = sma_tmp(dht.readTemperature() * 10.) / 10.;
    float h = sma_hmd(dht.readHumidity() * 10.) / 10.;

    static int counter;
    if (++counter == samples)
    {
      connect_wifi();
      String st = isnan(t) ? String("") : String(t);
      String sh = isnan(h) ? String("") : String(h);
      upload_sensor(st, sh);
      //disconnect_wifi();
      counter = 0;
    }
    
    startMillis = currentMillis;
   }
}
