#ifndef backend_h
#define backend_h

#include <WiFi.h>
#include <WiFiClientSecure.h>

class Backend
{
private:
    static const char _ssid[];
    static const char _password[];
    static const char _sensor_url[];

    WiFiClientSecure _wifiClient;

    static void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
    static void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
    static void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);

public:
    Backend();
    void connect_wifi();
    void disconnect_wifi();
    void upload_sensor(float v, int d, int z, int samples, int granularity);
};

#endif
