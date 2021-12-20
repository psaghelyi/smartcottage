
#include "Sensor.h"
#include "Backend.h"
#include "Avg.h"

Sensor sensor;
Backend backend;

void dump_debug(float v) {
  static float arr[100];
  static Avg avg(arr, 100);
  Serial.println(avg.mov_avg(v));
}

void setup() {
  Serial.begin(115200);
  pinMode(34,INPUT);
  backend.connect_wifi();
  Serial.print("Calibrated zero point: ");
  Serial.println(sensor.calibration());
}

void loop() {
  static unsigned long time_calibrate;
  if (millis() - time_calibrate > 1000 * 60 * 60) { // one hour
    Serial.print("Calibrated zero point: ");
    Serial.println(sensor.calibration());
    time_calibrate = millis();
  }
  
  // collect at least 300 samples to get valid measurement
  // (~370 is the usual at 50Hz)
  while (sensor.collect_samples() < 300)
  {}
  //sensor.dump();
  
  static float v;
  static int counter;
  static unsigned long time_upload;
  v += sensor.rms() * 0.5;
  counter++;
  if (millis() - time_upload > 5000) {
    v /= counter; 
    String sv = String(v);
    backend.upload_sensor(sv);
    // reset cycle
    v = 0;
    counter = 0;    
    time_upload = millis();
  }
  
  
  //delay(2000);
}
