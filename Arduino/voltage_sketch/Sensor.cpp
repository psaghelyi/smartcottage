#include "Sensor.h"

#include <climits>
#include <math.h>
#include <Arduino.h>


Sensor::Sensor() {
  _samples.reserve(500);  // ~370 for 50Hz
}

size_t Sensor::collect_samples(int zeroPoint) {
  bool in_wave = false;
  int prev = INT_MAX;
  
  _samples.clear();
  
  for(int i = 0; i < 500; i++) {
    int curr = analogRead(34);
    
    if (prev <= zeroPoint && curr > zeroPoint) {
      if (in_wave && _samples.size() > 300) {
        return _samples.size();
      }
      in_wave = true;
    }

    if (in_wave) {
      _samples.push_back(curr);
    }
    
    prev = curr;
  }
  
  return 0;
}

bool Sensor::compute(int zeroPoint) {
  _vpMin = INT_MAX;
  _vpMax = INT_MIN;
  
  // Calculate square.
  unsigned long square = 0; 
  for (auto it = std::begin(_samples); it != std::end(_samples); it++) {
    unsigned long q = (*it-zeroPoint) * (*it-zeroPoint);
    if (ULONG_MAX - square <= q) {
      //throw std::overflow_error("RMS calculation overflow");
      return false;
    }
    square += q;
    if (*it < _vpMin) _vpMin = *it;
    if (*it > _vpMax) _vpMax = *it;
  }
  // Calculate Mean.
  float mean = square / (float)_samples.size();
  // Calculate Root.
  _rms = sqrtf(mean);
  return true;
}

void Sensor::dump(int zeroPoint) {
  //Serial.println(_samples.size());
  for (auto it = std::begin(_samples); it != std::end(_samples); it++) {
    Serial.println(*it - zeroPoint);
  }
  Serial.flush();
}
