
#include "Sensor.h"
#include "Backend.h"

//#define DUMP

Sensor sensor;
Backend backend;

static void dump_debug(float v) {
  static SMA<20> filter;
  Serial.println(filter(v));
}

void setup() {
  Serial.begin(115200);
  pinMode(34,INPUT);
#ifndef DUMP
  backend.connect_wifi();
#endif

  Serial.println(sizeof(int));
}

void loop() {
  static float v;
  static int vpmin, vpmax;
  static int z = 3000;
  static SMA<10> filter_z;
  static int counter, g;
  static unsigned long time_upload = millis();
  
  // collect at least 300 samples to get valid measurement
  // (~370 is the usual at 50Hz)
  while (sensor.collect_samples(z) < 300)
  {    
  }

#ifdef DUMP
  sensor.dump();
  for (int i = 0; i < 10; ++i) Serial.println(0);
  delay(1000);
  
  //static SMA<20> movAvg1, movAvg2;
  //Serial.print(movAvg1(-sensor.vpMin()));
  //Serial.print("  ");
  //Serial.print(movAvg2(sensor.vpMax()));
  //Serial.print("  ");
  //Serial.println(sensor.zeroPoint());
  
  return;
#endif

  
  if (!sensor.compute(z)) {
    Serial.println("WTF");
  }

  v += sensor.rms();
  vpmin += sensor.vpMin();
  vpmax += sensor.vpMax();
  g += sensor.num_sample();
  counter++;
  if (millis() - time_upload > 5000) {
    v /= counter;
    vpmin /= counter;
    vpmax /= counter;
    g /= counter;
    z = filter_z((vpmin + vpmax) / 2);
    backend.upload_sensor(v, vpmax - vpmin, z, counter, g);
    // reset cycle
    v = 0.;
    vpmin = 0; vpmax = 0;
    counter = 0, g = 0;
    time_upload = millis();
  }

  
}
