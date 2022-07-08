// based on:
// https://tttapa.github.io/Pages/Mathematics/Systems-and-Control-Theory/Digital-filters/Simple%20Moving%20Average/C++Implementation.html

#ifndef sma_h
#define sma_h

#include <stdint.h>

template <uint8_t N, class input_t = uint16_t, class sum_t = uint32_t>
class SMA
{
public:
    SMA(input_t init = 0) : sum(init * N)
    {
        for (int i = 0; i < N; i++)
        {
            previousInputs[i] = init;
        }
    }

    input_t operator()(input_t input)
    {
        sum -= previousInputs[index];
        sum += input;
        previousInputs[index] = input;
        if (++index == N)
            index = 0;
        return (sum + (N / 2)) / N;
    }

    static_assert(
        sum_t(0) < sum_t(-1), // Check that `sum_t` is an unsigned type
        "Error: sum data type should be an unsigned integer, otherwise, "
        "the rounding operation in the return statement is invalid.");

private:
    uint8_t index = 0;
    input_t previousInputs[N] = {};
    sum_t sum = 0;
};

#endif
