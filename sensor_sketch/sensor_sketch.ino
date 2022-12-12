#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <Pinger.h>

#include "avg.h"
#include "logger.h"
#include "config.h"

#define HOST_NAME "ESP01-"SENSOR_NAME

#define VERSION "2022-12-12.v3"

#define DHTPIN 2
#define DHTTYPE DHT22

DHT                 dht(DHTPIN, DHTTYPE);
WiFiClientSecure    wifiClient;
WiFiUDP             ntpUDP;
NTPClient           timeClient(ntpUDP, "europe.pool.ntp.org", 7200, 120000);  // +2h, update 2mins
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


void restart() {
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < 5; ++i) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
  }
  ESP.restart();
}


void connect_wifi() {
  currentMillis = millis();
  if (currentMillis - startWifiMillis < 30000)
    return;

  if (WiFi.status() == WL_CONNECTED)
  {
    IPAddress gateway = WiFi.gatewayIP();
    logger.print(timeClient.getFormattedTime());
    logger.print(" [ping] ");
    logger.print(gateway.toString());
    logger.print(" result: ");
    currentMillis = millis();
    if (pinger.Ping(gateway))
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
  WiFi.setHostname(HOST_NAME);
  WiFi.begin(ssid, pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    logger.print(timeClient.getFormattedTime());
    logger.println(" Connection Failed! Rebooting...");
    restart();
  }

  //WiFi.setAutoReconnect(true);
  //WiFi.persistent(true);
  timeClient.begin();
  timeClient.update();
  
  logger.print(timeClient.getFormattedTime());
  logger.print(" IP=");
  logger.print(WiFi.localIP().toString());
  logger.print(" Mask=");
  logger.print(WiFi.subnetMask().toString());
  logger.print(" Gateway=");
  logger.print(WiFi.gatewayIP().toString());
  logger.print(" DNS=");
  logger.println(WiFi.dnsIP().toString());

  wifiClient.setInsecure();

  ArduinoOTA.setHostname(HOST_NAME);
  ArduinoOTA.begin();
  MDNS.begin(HOST_NAME);

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
    restart();
  }  
}


void handleRoot() {
  webServer.send(200, "text/plain; charset=UTF-8", logger.getData());
}

void handleVersion() {
  webServer.send(200, "text/plain", VERSION);
}

void handleNotFound(){
  webServer.send(404, "text/plain", "404: Not found");
}


void setup() {
  logger.print(timeClient.getFormattedTime());
  logger.print(" version=");
  logger.println(VERSION);

  webServer.on("/", HTTP_GET, handleRoot);
  webServer.on("/version", HTTP_GET, handleVersion);
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

  static AVG<samples * 2> avg_tmp;
  static AVG<samples * 2> avg_hmd;
  static bool init;
  if (!init)
  {
    avg_tmp.reset(dht.readTemperature());
    avg_hmd.reset(dht.readHumidity());
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
    logger.print("˚C h=");
    logger.print(h);
    logger.println("%");
    
    t = avg_tmp(t);
    h = avg_hmd(h);
    
    static int counter = samples - 1;
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
