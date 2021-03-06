#include <climits>
#include <math.h>
#include <Arduino.h>

#include "sensor.h"

Sensor::Sensor()
{
    _samples.reserve(500); // ~370 for 50Hz
}

size_t Sensor::collect_samples(int zeroPoint)
{
    int prev = analogRead(_pin);
    bool start_below = prev < zeroPoint; // wave starts from negative
    bool in_wave = false;

    _samples.clear();

    for (int i = 0; i < 500; i++)
    {
        int curr = analogRead(_pin);

        if ((start_below && prev < zeroPoint && curr >= zeroPoint) ||
            (prev >= zeroPoint && curr < zeroPoint))
        {
            if (in_wave && _samples.size() > 350)
            {
                return _samples.size();
            }
            in_wave = true;
        }

        if (in_wave)
        {
            _samples.push_back(curr);
        }

        prev = curr;
    }

    return 0;
}

bool Sensor::compute(int zeroPoint)
{
    _vpMin = INT_MAX;
    _vpMax = INT_MIN;

    // Calculate square.
    unsigned long square = 0;
    for (int sample : _samples)
    {
        unsigned long q = (sample - zeroPoint) * (sample - zeroPoint);
        if (ULONG_MAX - square <= q)
        {
            // throw std::overflow_error("RMS calculation overflow");
            return false;
        }
        square += q;
        if (sample < _vpMin) _vpMin = sample;
        if (sample > _vpMax) _vpMax = sample;
    }
    // Calculate Mean.
    float mean = square / (float)_samples.size();
    // Calculate Root.
    _rms = sqrtf(mean);
    return true;
}

void Sensor::dump(int zeroPoint)
{
    // Serial.println(_samples.size());
    for (int sample : _samples)
    {
        Serial.println(sample - zeroPoint);
    }
    Serial.flush();
}
