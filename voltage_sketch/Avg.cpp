#include "Avg.h"

Avg::Avg(float *arr, int narr) 
    : _n(0), _movn(0), _arr(arr), _narr(narr) 
  {
  }
  
float Avg::inc_avg(float v) 
{
  _a = (_a * _n + v) / ++_n;
  return _a;
}

float Avg::mov_avg(float v) {
  _arr[_n] = v;
  
  if (++_n == _narr) {
    _n = 0;
  }
  
  if (_movn < _narr) {
    _movn++;
  }

  _a = 0.;
  for (int i = 0; i < _narr; ++i) {
    _a += _arr[i];
  }
  
  return _a / _movn;
}
