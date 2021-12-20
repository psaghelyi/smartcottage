#include "Backend.h"


#include <HTTPClient.h>

const char Backend::_ssid[] = "DHARMA_NET";
const char Backend::_password[] = "JuanValdes5";
const char Backend::_sensor_url[] = "https://psaghelyi.ddns.net:12345/sensor/v1";


Backend::Backend() {
}


void Backend::connect_wifi() {
  if (WiFi.status() == WL_CONNECTED)
    return;

  Serial.println("Connecting to wifi: ");
  Serial.println(_ssid);
  Serial.flush();
  WiFi.begin(_ssid, _password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    Serial.flush();
  }
  Serial.println(" IP address: ");
  Serial.println(WiFi.localIP());
  Serial.flush();

  _wifiClient.setInsecure();
}

void Backend::disconnect_wifi() {
  WiFi.disconnect();
}


void Backend::upload_sensor(String const & sv)
{
  String body = "{\"voltage\": " + sv + "}";
  Serial.print(body);
  Serial.print(" --> ");
    
  HTTPClient http;
  http.begin(_wifiClient, _sensor_url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.PUT(body);
  http.end();

  Serial.println(httpResponseCode);
  Serial.flush();
}
