#ifndef Avg_h
#define Avg_h


class Avg
{
protected:
    float _a;
    int _n;
public:
  Avg();
  virtual ~Avg();
  virtual float eval(float v) = 0;
};

class IncAvg : public Avg
{
public:
  IncAvg();
  float eval(float v);
};

class MovAvg : public Avg
{
private:
  float *_arr;
  int _narr;
  int _movn;    
public:
  MovAvg(int narr);
  ~MovAvg();
  float eval(float v);
};

#endif Avg
