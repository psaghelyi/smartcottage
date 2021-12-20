#ifndef Backend_h
#define Backend_h

#include <WiFi.h>
#include <WiFiClientSecure.h>


class Backend
{
  private:
    static const char _ssid[];
    static const char _password[];
    static const char _sensor_url[];

    WiFiClientSecure _wifiClient;
  public:
    Backend();
    void connect_wifi();
    void disconnect_wifi();
    void upload_sensor(String const & sv);
    
};

#endif
