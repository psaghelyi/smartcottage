#ifndef sensor_h
#define sensor_h

#include <vector>

class Sensor
{
private:
    const int _pin = 34;
    std::vector<int> _samples;
    int _vpMin, _vpMax;
    float _rms = 0.;

public:
    Sensor();
    std::size_t collect_samples(int zeroPoint);
    bool compute(int zeroPoint);
    void dump(int zeroPoint);

    std::size_t num_sample() const { return _samples.size(); }
    const std::vector<int> & samples() const { return _samples; }
    int vpMin() const { return _vpMin; }
    int vpMax() const { return _vpMax; }
    float rms() const { return _rms; }
};

#endif
