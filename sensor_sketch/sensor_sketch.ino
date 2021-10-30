#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <DHT.h>

WiFiClientSecure wifiClient;

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);


const char* ssid = "DHARMA_NET";
const char* password = "JuanValdes5";
const char* sensor_url = "https://psaghelyi.ddns.net:12345/sensor/s1";

const int dataPostDelay = 10 * 60 * 1000;  // 10 minutes


void setup() {
  Serial.begin(115200);
  dht.begin();
}

void connect_wifi() {
  if (WiFi.status() == WL_CONNECTED)
    return;

  Serial.println("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    Serial.flush();
  }
  Serial.println(" IP address: ");
  Serial.println(WiFi.localIP());
  Serial.flush();

  wifiClient.setInsecure();
}

void disconnect_wifi() {
  WiFi.disconnect();
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

void loop()
{
  while (true) {
    connect_wifi();    
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    String st = isnan(t) ? String("") : String(t);
    String sh = isnan(h) ? String("") : String(h);
    upload_sensor(st, sh);
    disconnect_wifi();
    delay(dataPostDelay);
  }
}
