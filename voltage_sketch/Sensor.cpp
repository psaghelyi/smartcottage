#include "Sensor.h"
#include "Avg.h"

#include <climits>
#include <stdexcept>
#include <math.h>
#include <Arduino.h>


Sensor::Sensor() {
  _samples.reserve(500);  // ~370
}

int Sensor::deviation() {
  int mi = INT_MAX, ma = 0;
  for (auto it = std::begin(_samples); it != std::end(_samples); it++) {
    if (*it < mi) mi = *it;
    if (*it > ma) ma = *it;
  }
  return ma-mi;
}

float Sensor::rms()
{
    // Calculate square.
    unsigned long square = 0; 
    for (auto it = std::begin(_samples); it != std::end(_samples); it++) {
        unsigned long q = *it * *it;
        if (ULONG_MAX - square <= q)
          throw std::overflow_error("RMS calculation overflow");
        square += q;
    }
 
    // Calculate Mean.
    float mean = square / (float)_samples.size();
 
    // Calculate Root.
    float root = sqrtf(mean);
 
    return root;
}

int Sensor::calibration() {
  Avg avg;
  float zeroPoint;
  for (int i = 0; i < 100000; ++i) {
    zeroPoint = avg.inc_avg(analogRead(34));
  }
  _zeroPoint = zeroPoint;
  return _zeroPoint;
}

int Sensor::collect_samples() {
  bool in_wave = false;
  int prev = INT_MAX;
  
  _samples.clear();  
  for(;;) {
    int curr = analogRead(34) - _zeroPoint;
    
    if (prev <= 0 && curr > 0) {
      if (!in_wave) {
        in_wave = true; 
      } else {
        break;
      }
    }

    if (in_wave) {
      _samples.push_back(curr);
    }
    
    prev = curr;
  }
  
  return _samples.size();
}

void Sensor::dump() {
  //Serial.println(_samples.size());
  for (auto it = std::begin(_samples); it != std::end(_samples); it++) {
    Serial.println(*it);
  }
  Serial.flush();
}
