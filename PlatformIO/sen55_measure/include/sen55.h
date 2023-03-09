#ifndef sen55_h
#define sen55_h

#include <SensirionI2CSen5x.h>
#include <Wire.h>
#include "avg.h"

class Sen55
{
private:
    static SensirionI2CSen5x _sen5x;
    static void printModuleVersions();
    static void printSerialNumber();
    
    static const uint8_t N = 10;

    AVG<N> _avg_pm1p0;
    AVG<N> _avg_pm2p5;
    AVG<N> _avg_pm4p0;
    AVG<N> _avg_pm10p0;
    AVG<N> _avg_humidity;
    AVG<N> _avg_temperature;

public:
    Sen55();
    void init();
    String read();
    String dump();
};

#endif