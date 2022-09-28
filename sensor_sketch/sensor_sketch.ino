#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <Pinger.h>

#include "sma.h"
#include "logger.h"
#include "config.h"

#define OTA_Host_Name "ESP01-"SENSOR_NAME

#define DHTPIN 2
#define DHTTYPE DHT22

DHT                 dht(DHTPIN, DHTTYPE);
WiFiClientSecure    wifiClient;
WiFiUDP             ntpUDP;
NTPClient           timeClient(ntpUDP);
ESP8266WebServer    webServer(80);

const char*         ssid = SSID;
const char*         pass = PASS;
const char*         sensor_url = SENSOR_URL;

Logger<4096>        logger;
Pinger              pinger;

unsigned long       startSensorMillis = millis();
unsigned long       startWifiMillis = ULONG_MAX;
unsigned long       currentMillis;
const unsigned long samples = 10;
const unsigned long rate = 10 * 60 * 1000 / samples;  // 10 minutes - 10 samples


void connect_wifi() {
  currentMillis = millis();
  if (currentMillis - startWifiMillis < 30000)
    return;

  if (WiFi.status() == WL_CONNECTED)
  {
    IPAddress gateway = WiFi.gatewayIP();
    logger.print(timeClient.getFormattedTime());
    logger.print(" Ping ");
    logger.print(gateway.toString());
    logger.print(" result: ");
    currentMillis = millis();
    if (pinger.Ping(WiFi.gatewayIP()))
    {
      logger.print("succeed  (");
      logger.print(millis() - currentMillis);
      logger.println(" ms)");
      startWifiMillis = currentMillis;
      return;
    }
    logger.println("failed !!!");
  }

  logger.print(timeClient.getFormattedTime());
  logger.print(" Connecting to wifi: ");
  logger.println(ssid);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    logger.print(timeClient.getFormattedTime());
    logger.println(" Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  //WiFi.setAutoReconnect(true);
  //WiFi.persistent(true);
  timeClient.begin();
  timeClient.update();
  
  logger.print(timeClient.getFormattedTime());
  logger.print(" IP address: ");
  logger.println(WiFi.localIP().toString());
  
  wifiClient.setInsecure();

  ArduinoOTA.setHostname(OTA_Host_Name);
  ArduinoOTA.begin();
  MDNS.begin(OTA_Host_Name);

  startWifiMillis = currentMillis;
}


void upload_sensor(String const & st, String const & sh)
{
  String body = "{\"temperature\": " + st + ", \"humidity\": " + sh + "}";
  logger.print(timeClient.getFormattedTime());
  logger.print(" [send] ");
  logger.print(body);
  logger.print(" --> ");
    
  HTTPClient http;
  http.begin(wifiClient, sensor_url);
  http.addHeader("Content-Type", "application/json");
  currentMillis = millis();
  int httpResponseCode = http.PUT(body);
  unsigned long httpResponseTime = millis() - currentMillis;
  http.end();

  logger.print(httpResponseCode);
  logger.print("  (");
  logger.print(httpResponseTime);
  logger.println(" ms)");

  static int error_counter;
  if (httpResponseCode == 200)
  {
    error_counter = 0;
    return;
  }
  
  if (++error_counter >= 3)
  {
    logger.print(timeClient.getFormattedTime());
    logger.println(" Uploading Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }  
}


void handleRoot() {
  webServer.send(200, "text/plain", logger.getData());
}


void handleNotFound(){
  webServer.send(404, "text/plain", "404: Not found");
}


void setup() {
  webServer.on("/", HTTP_GET, handleRoot);
  webServer.onNotFound(handleNotFound);
  webServer.begin();

  dht.begin();
  // dht needs 2 seconds to settle
  do {
    delay(2500);
  }
  while (dht.readTemperature() > 80.);  // sometimes even more...
}


void loop()
{
  connect_wifi();
  if (WiFi.status() != WL_CONNECTED)
    return;

  ArduinoOTA.handle();
  webServer.handleClient();
  timeClient.update();

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
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    
    logger.print(timeClient.getFormattedTime());
    logger.print(" [read] t=");
    logger.print(t);
    logger.print("; h=");
    logger.println(h);
    
    t = sma_tmp(t * 10.) / 10.;
    h = sma_hmd(h * 10.) / 10.;
    
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
