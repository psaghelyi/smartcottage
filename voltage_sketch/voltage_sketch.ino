
#include "Sensor.h"
#include "Backend.h"
#include "Avg.h"

//#define DUMP

Sensor sensor;
Backend backend;

static void dump_debug(float v) {
  static MovAvg movAvg(100);
  Serial.println(movAvg.eval(v));
}

void setup() {
  Serial.begin(115200);
  pinMode(34,INPUT);
  Serial.print("Calibrated zero point: ");
  Serial.println(sensor.calibration());
  backend.connect_wifi();
}

void loop() {
  static unsigned long time_calibrate = millis();
  if (millis() - time_calibrate > 1000 * 60 * 60) { // one hour
    Serial.print("Calibrated zero point: ");
    Serial.println(sensor.calibration());
    time_calibrate = millis();
  }
  
  // collect at least 300 samples to get valid measurement
  // (~370 is the usual at 50Hz)
  while (sensor.collect_samples() < 300)
  {}

#ifdef DUMP
  sensor.dump();
  for (int i = 0; i < 10; ++i) Serial.println(0);
  delay(3000);
  return;
#endif

  static float v, d;
  static int counter, g;
  static unsigned long time_upload = millis();
  v += sensor.rms();
  d += sensor.vpp();
  g += sensor.num_sample();
  counter++;
  if (millis() - time_upload > 5000) {
    v /= counter; 
    d /= counter;
    g /= counter;
    backend.upload_sensor(v, d, sensor.zeroPoint(), counter, g);
    // reset cycle
    v = 0., d = 0.;
    counter = 0, g = 0;
    time_upload = millis();
  }
}
