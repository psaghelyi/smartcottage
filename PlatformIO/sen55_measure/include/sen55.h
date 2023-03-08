#ifndef sen55_h
#define sen55_h

#include <SensirionI2CSen5x.h>
#include <Wire.h>


class Sen55
{
private:
    static SensirionI2CSen5x _sen5x;
    static void printModuleVersions();
    static void printSerialNumber();

public:
    Sen55();
    void init();
    String read();
};

#endif