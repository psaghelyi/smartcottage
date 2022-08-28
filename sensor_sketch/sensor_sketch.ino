#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <Pinger.h>
extern "C"
{
  #include <lwip/icmp.h> // needed for icmp packet definitions
}
#include "sma.h"
#include "config.h"

#define OTA_Host_Name "ESP01-"SENSOR_NAME

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

WiFiClientSecure wifiClient;

ESP8266WebServer server(80);

const char* ssid = SSID;
const char* pass = PASS;
const char* sensor_url = SENSOR_URL;


unsigned long startSensorMillis = millis();
unsigned long startWifiMillis = ULONG_MAX;
unsigned long currentMillis;
const unsigned long samples = 10;
const unsigned long rate = 10 * 60 * 1000 / samples;  // 10 minutes - 10 samples

Pinger pinger;

void connect_wifi() {
  currentMillis = millis();
  if (currentMillis - startWifiMillis < 30000)
    return;

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("Ping test result: ");
    if (pinger.Ping(WiFi.gatewayIP()))
    {
      Serial.println("succeed");
      Serial.flush();
      return;
    }
    Serial.println("failed !!!");
    Serial.flush();  
  }
  
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.flush();

  wifiClient.setInsecure();

  ArduinoOTA.setHostname(OTA_Host_Name);
  ArduinoOTA.begin();
  MDNS.begin(OTA_Host_Name);

  startWifiMillis = currentMillis;
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

void handleRoot() {
  server.send(200, "text/plain", "Hello");
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found");
}

void setup() {
  Serial.begin(115200);

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();

  dht.begin();
  // dht needs 2 seconds to settle
  do {
    delay(2000);
  }
  while (dht.readTemperature() > 80.);
}

void loop()
{
  connect_wifi();
  if (WiFi.status() != WL_CONNECTED)
    return;
    
  ArduinoOTA.handle();
  server.handleClient();
  
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
  if (currentMillis - startSensorMillis >= rate)
  {        
    float t = sma_tmp(dht.readTemperature() * 10.) / 10.;
    float h = sma_hmd(dht.readHumidity() * 10.) / 10.;

    static int counter;
    if (++counter == samples)
    {
      String st = isnan(t) ? String("") : String(t);
      String sh = isnan(h) ? String("") : String(h);
      upload_sensor(st, sh);
      //disconnect_wifi();
      counter = 0;
    }
    
    startSensorMillis = currentMillis;
  }
}
