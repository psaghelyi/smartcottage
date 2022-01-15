#ifndef sensor_h
#define sensor_h

#include <vector>
#include "sma.h"

class Sensor
{
private:
    std::vector<int> _samples;
    SMA<20> _filter;

    const int _pin = 34;
    int _vpMin, _vpMax;
    float _rms = 0.;

public:
    Sensor();
    std::size_t collect_samples(int zeroPoint);
    bool compute(int zeroPoint);
    void dump(int zeroPoint);

    std::size_t num_sample() const { return _samples.size(); }
    int vpMin() const { return _vpMin; }
    int vpMax() const { return _vpMax; }
    float rms() const { return _rms; }
};

#endif
