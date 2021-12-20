#ifndef Sensor_h
#define Sensor_h

#include <vector>

class Sensor
{
private:
  int _zeroPoint = 3000;
  std::vector<int> _samples;
  
public:
  Sensor();
  int deviation();
  float rms();
  int collect_samples();
  int calibration();
  void dump();
};

#endif
