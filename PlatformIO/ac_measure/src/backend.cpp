#include "backend.h"

#include <HTTPClient.h>

void Payload::reset()
{
    rms = 0.;
    vpmin = 0;
    vpmax = 0;
    samples = 0;
    granularity = 0;
}

void Payload::normalize()
{
    rms /= samples;
    vpmin /= samples;
    vpmax /= samples;
    granularity /= samples;
    zero = (vpmin + vpmax) / 2;
}

// const char Backend::_ssid[] = "TP-LINK_M5250_30228B";
// const char Backend::_password[] = "21275721";
const char Backend::_ssid[] = "DHARMA_NET";
const char Backend::_password[] = "JuanValdes5";
const char Backend::_sensor_url[] = "https://psaghelyi.ddns.net:12345/power/v1";

WiFiClientSecure Backend::_wifiClient;

Backend::Backend()
{
    // delete old config
    WiFi.disconnect(true);
}

void Backend::WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("Connected to AP successfully!");
}

void Backend::WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void Backend::WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("Disconnected from WiFi access point");
    Serial.print("WiFi lost connection. Reason: ");
    Serial.println(info.disconnected.reason);
    Serial.println("Trying to Reconnect");
    WiFi.begin(_ssid, _password);
}

void Backend::connect_wifi()
{
    WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
    WiFi.onEvent(WiFiGotIP, SYSTEM_EVENT_STA_GOT_IP);
    WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);

    if (WiFi.status() == WL_CONNECTED)
        return;

    Serial.println("Connecting to wifi: ");
    Serial.println(_ssid);
    Serial.flush();
    WiFi.begin(_ssid, _password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        Serial.flush();
    }

    _wifiClient.setInsecure();
}

void Backend::disconnect_wifi()
{
    WiFi.disconnect();
}

int Backend::send_request(const String &body)
{
    HTTPClient http;
    http.begin(_wifiClient, _sensor_url);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.PUT(body);
    http.end();
    return httpResponseCode;
}

void Backend::upload_measurement(const Payload &payload, const std::vector<int> &samples)
{
    String body = "{\"voltage\": " + String(payload.rms) +
                  ", \"peak2peak\": " + String(payload.vpmax - payload.vpmin) +
                  ", \"zero\": " + String(payload.zero) +
                  ", \"samples\": " + String(payload.samples) +
                  ", \"granularity\": " + String(payload.granularity);

    String log = body;
    
    body += ", \"raw\": [" + String(payload.zero) + ", ";
    for (int sample : samples)
    {
        body += String(sample) + ", ";
    }
    body += String(payload.zero) + "]}";

    Serial.print(millis());
    Serial.print(": ");
    Serial.print(body);
    Serial.print(" --> ");
    int httpResponseCode = send_request(body);
    Serial.println(httpResponseCode);
    Serial.flush();
}
