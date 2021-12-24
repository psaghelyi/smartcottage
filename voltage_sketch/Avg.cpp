#include "Avg.h"

Avg::Avg() : _a(0.), _n(0) 
{
}

Avg::~Avg()
{
}

IncAvg::IncAvg() {
}

float IncAvg::eval(float v) 
{
  _a = (_a * _n + v) / ++_n;
  return _a;
}

MovAvg::MovAvg(int narr) 
: _narr(narr), _movn(0) 
{
  _arr = new float[_narr];
}

MovAvg::~MovAvg()
{
  delete[] (_arr);
}

float MovAvg::eval(float v) {
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
