#ifndef avg_h
#define avg_h

#include <stdint.h>

template <uint8_t N, class input_t = float, class sum_t = float>
class AVG
{
public:
    input_t operator()(input_t input)
    {
        sum -= data[index];
        sum += input;
        data[index] = input;
        if (++index == N)
          index = 0;

        n = n < N ? n + 1 : N;
        return sum / n;
    }

    input_t operator[](uint8_t i)
    {
        return data[i];
    }

private:
    input_t data[N] = {};
    uint8_t index = 0;
    uint8_t n = 0;
    sum_t sum = 0;
};

#endif