#ifndef avg_h
#define avg_h

#include <stdint.h>

template <uint8_t N, class input_t = float, class sum_t = float>
class AVG
{
public:
    AVG(input_t init = 0)
    {
        reset(init);
    }

    void reset(input_t init = 0)
    {
        for (int i = 0; i < N; i++)
        {
            data[i] = init;
            sum += init;
        }
    }

    input_t operator()(input_t input)
    {
        sum += input - data[index];
        data[index] = input;
        if (++index == N)
          index = 0;

        return sum / N;
    }

private:
    uint8_t index = 0;
    input_t data[N] = {};
    sum_t sum = 0;
};

#endif