#include <Arduino.h>

#include "sma.h"
#include "sensor.h"
#include "backend.h"

//#define DUMP
//#define DEBUG

static Backend backend;
static Sensor sensor1, sensor2, *psensor = &sensor1;
static Payload payload1, payload2, *ppayload = &payload1;
static bool do_upload = false;

static void debug_print(String s)
{
#ifdef DEBUG
    Serial.print(millis());
    Serial.print(": ");
    Serial.println(s);
    Serial.flush();
#endif
}

static void swap_storage()
{
    if (psensor == &sensor1)
    {
        psensor = &sensor2;
        ppayload = &payload2;
    } else {
        psensor = &sensor1;
        ppayload = &payload1;
    }
    do_upload = true;
}

void loop()
{
    static int zeroPoint = 3000;
    static SMA<10> filter_z(zeroPoint);

    Sensor &sensor = *psensor;
    Payload &payload = *ppayload;

    while (true)
    {
        if (!sensor.collect_samples(zeroPoint))
        {
            Serial.println("Irregular wave pattern");
            continue;
        }
        break;
    }

#ifdef DUMP
    sensor.dump(zeroPoint);
    for (int i = 0; i < 10; ++i)
        Serial.println(0);
    
    return;
#endif

    if (!sensor.compute(zeroPoint))
    {
        Serial.println("RMS calculation overflow");
        return;
    }

    payload.rms = sensor.rms();
    payload.vpmin = sensor.vpMin();
    payload.vpmax = sensor.vpMax();
    payload.zero = (sensor.vpMin() + sensor.vpMax()) / 2;
    payload.granularity = sensor.num_sample();
    payload.samples = CYCLE_COUNT;
    zeroPoint = filter_z(payload.zero);
    swap_storage();
}

static void loop0()
{
    if (!do_upload)
    {
        delay(500);
        return;
    }

    if (psensor == &sensor1)
    {
        backend.upload_measurement(payload2, sensor2.samples());
    } else {
        backend.upload_measurement(payload1, sensor1.samples());
    }
    do_upload = false;
}

static void core0code(void *pvParameters)
{
    while(true) loop0();
}

void setup()
{
    Serial.begin(115200);
    pinMode(34, INPUT);
#ifndef DUMP
    backend.connect_wifi();
#endif
    
    xTaskCreatePinnedToCore(core0code, "Task0", 10000, NULL, 1, NULL,  0); 
    delay(500);
}
