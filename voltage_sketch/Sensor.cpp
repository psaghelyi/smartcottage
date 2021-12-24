#include "Sensor.h"
#include "Avg.h"

#include <climits>
#include <stdexcept>
#include <math.h>
#include <Arduino.h>


Sensor::Sensor() {
  _samples.reserve(500);  // ~370 for 50Hz
}

int Sensor::vpp() {
  int mi = INT_MAX, ma = INT_MIN;
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
  IncAvg incAvg;
  float zeroPoint;
  for (int i = 0; i < 100000; ++i) {
    zeroPoint = incAvg.eval(analogRead(34));
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
      if (in_wave) {
        break;
      }
      in_wave = true;
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
