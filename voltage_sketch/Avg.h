#ifndef Avg_h
#define Avg_h


class Avg
{
private:
    int _n;
    int _movn;
    int _narr;
    float _a;
    float *_arr;

public:
  Avg(float *arr = nullptr, int narr = 0);
  float inc_avg(float v);
  float mov_avg(float v); 
};


#endif Avg
