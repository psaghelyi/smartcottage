#include <climits>
#include <math.h>
#include <Arduino.h>

#include "sensor.h"

Sensor::Sensor()
{
}

bool Sensor::collect_samples(int zeroPoint)
{
    _samples.fill(0);

    // find the starting point of the period
    int prev = analogRead(_pin);
    while (true)
    {
        int curr = analogRead(_pin);
        if (prev <= zeroPoint && curr > zeroPoint)
        {
            break;
        }
        prev = curr;
    }

    // collect samples
    _num_samples = 0;
    for (int j = 0; j < CYCLE_COUNT; j++)
    {
        SampleArray::size_type i = 0;
        for (; i < _samples.max_size(); i++)
        {
            int curr = analogRead(_pin);
            if (prev <= zeroPoint && curr > zeroPoint)
            {
                if (i > 300)
                {
                    break;
                }
            }
            _samples[i] += curr;
            prev = curr;
        }
        if (i == _samples.max_size())
        {
            return false; // is this a good idea to drop everything? 
        }
        _num_samples += i;
    }

    _num_samples /= CYCLE_COUNT;
    for (SampleArray::value_type & sample : _samples)
    {
        sample /= CYCLE_COUNT;
    }
    return true;
}

bool Sensor::compute(int zeroPoint)
{
    _vpMin = INT_MAX;
    _vpMax = INT_MIN;

    // Calculate square.
    unsigned long square = 0;
    for (int i = 0; i < _num_samples; i++)
    {
        int sample = _samples[i];
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
    float mean = square / (float)_num_samples;
    
    // Calculate Root.
    _rms = sqrtf(mean);
    return true;
}

void Sensor::dump(int zeroPoint)
{
    // Serial.println(_num_samples);
    for (int i = 0; i < _num_samples; i++)
    {
        Serial.println(_samples[i] - zeroPoint);
    }
    Serial.flush();
}
