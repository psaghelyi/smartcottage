// based on:
// https://tttapa.github.io/Pages/Mathematics/Systems-and-Control-Theory/Digital-filters/Simple%20Moving%20Average/C++Implementation.html

#ifndef sma_h
#define sma_h

#include <stdint.h>

template <uint8_t N>
class SMA
{
public:
    SMA(float init = 0) : sum(init * N)
    {
        for (int i = 0; i < N; i++)
        {
            previousInputs[i] = init;
        }
    }

    float operator()(float input)
    {
        sum -= previousInputs[index];
        sum += input;
        previousInputs[index] = input;
        if (++index == N)
            index = 0;
        return (sum + (N / 2)) / N;
    }

private:
    uint8_t index = 0;
    float previousInputs[N] = {};
    float sum = 0;
};

#endif
