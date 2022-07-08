#include <Arduino.h>

#include "sma.h"
#include "sensor.h"
#include "backend.h"

//#define DUMP
//#define DEBUG

#define CYCLE_MS 5000

static Backend backend;
static Sensor sensor1, sensor2, *psensor = &sensor1;
static Payload payload1, payload2, *ppayload = &payload1;
static bool do_upload = false;

static void debug_print(String s)
{
#ifdef DEBUG
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
    ppayload->reset();
}

void loop()
{
    static int zeroPoint = 3000;
    static SMA<10> filter_z(zeroPoint);
    static unsigned long time_upload = millis();

    Sensor &sensor = *psensor;
    Payload &payload = *ppayload;

    // collect at least 300 samples to get valid measurement
    // (~370 is the usual at 50Hz)
    while (sensor.collect_samples(zeroPoint) < 300)
    {
    }

#ifdef DUMP
    sensor.dump(z);
    for (int i = 0; i < 10; ++i)
        Serial.println(0);
    delay(1000);

    // static SMA<20> movAvg1, movAvg2;
    // Serial.print(movAvg1(-sensor.vpMin()));
    // Serial.print("  ");
    // Serial.print(movAvg2(sensor.vpMax()));
    // Serial.print("  ");
    // Serial.println(sensor.zeroPoint());

    return;
#endif

    if (!sensor.compute(zeroPoint))
    {
        Serial.println("WTF");
    }

    payload.rms += sensor.rms();
    payload.vpmin += sensor.vpMin();
    payload.vpmax += sensor.vpMax();
    payload.granularity += sensor.num_sample();
    payload.samples++;
    if (millis() - time_upload > CYCLE_MS)
    {
        payload.normalize();
        zeroPoint = filter_z(payload.zero);
        swap_storage();
        // reset cycle
        time_upload = millis();
    }
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
