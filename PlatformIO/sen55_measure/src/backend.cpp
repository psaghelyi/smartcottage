#include "backend.h"
#include "config.h"

#include <HTTPClient.h>

// const char Backend::_ssid[] = "TP-LINK_M5250_30228B";
// const char Backend::_password[] = "21275721";
const char Backend::_ssid[] = SSID;
const char Backend::_password[] = PASS;
const char Backend::_sensor_url[] = SENSOR_URL;

WiFiClientSecure Backend::_wifiClient;

Backend::Backend(const char *hostname)
{
    // delete old config
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(hostname);
}

void Backend::WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.print(millis());
    Serial.print(": ");
    Serial.println("Connected to AP successfully!");
    Serial.print(millis());
    Serial.print(": ");
    Serial.print("SSID: ");
    Serial.print((char*)info.wifi_sta_connected.ssid);
    Serial.print(" Channel: ");
    Serial.println(info.wifi_sta_connected.channel);
}

void Backend::WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.print(millis());
    Serial.print(": ");
    Serial.print("IP address: ");
    Serial.print(IPAddress(info.got_ip.ip_info.ip.addr).toString());
    Serial.print(" netmask: ");
    Serial.print(IPAddress(info.got_ip.ip_info.netmask.addr).toString());
    Serial.print(" GW address: ");
    Serial.println(IPAddress(info.got_ip.ip_info.gw.addr).toString());
}

void Backend::WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.print(millis());
    Serial.print(": ");
    Serial.println("Disconnected from WiFi access point");
    Serial.print(millis());
    Serial.print(": ");
    Serial.print("WiFi lost connection. Reason: ");
    Serial.println(info.wifi_sta_disconnected.reason);
    Serial.print(millis());
    Serial.print(": ");
    Serial.print("Trying to reconnect...");
    if (!WiFi.reconnect())
    {

        Serial.println("Failed.");
        Serial.println("Restarting...");
        delay(5000);
        ESP.restart();
    }
    Serial.println("Succeeded.");
}

void Backend::connect_wifi()
{
    WiFi.disconnect(true);
    delay(1000);

    WiFi.onEvent(WiFiStationConnected, ARDUINO_EVENT_WIFI_STA_CONNECTED);
    WiFi.onEvent(WiFiGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(WiFiStationDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    if (WiFi.status() == WL_CONNECTED)
        return;

    Serial.print(millis());
    Serial.print(": ");
    Serial.print("Connecting to wifi: ");
    Serial.println(_ssid);
    Serial.flush();
    WiFi.begin(_ssid, _password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("Restarting...");
        delay(5000);
        ESP.restart();
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

void Backend::upload(const String &data) const
{    
    Serial.print(millis());
    Serial.print(": ");
    Serial.print(data);
    Serial.print(" --> ");
    int httpResponseCode = send_request(data);
    Serial.println(httpResponseCode);
    Serial.flush();
}
