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
  int vpp();
  float rms();
  int collect_samples();
  int calibration();
  int zeroPoint() const { return _zeroPoint; }
  int num_sample() const { return _samples.size(); }
  void dump();
};

#endif
