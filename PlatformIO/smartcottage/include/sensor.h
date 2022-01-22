#ifndef sensor_h
#define sensor_h

#include "common.h"

class Sensor
{
private:
    const int _pin = 34;
    SampleArray _samples;
    SampleArray::size_type _num_samples;
    int _vpMin, _vpMax;
    float _rms = 0.;

public:
    Sensor();
    bool collect_samples(int zeroPoint);
    bool compute(int zeroPoint);
    void dump(int zeroPoint);

    std::size_t num_sample() const { return _num_samples; }
    const SampleArray & samples() const { return _samples; }
    int vpMin() const { return _vpMin; }
    int vpMax() const { return _vpMax; }
    float rms() const { return _rms; }
};

#endif
