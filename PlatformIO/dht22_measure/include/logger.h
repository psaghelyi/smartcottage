#ifndef logger_h
#define logger_h

template <unsigned int N>
class Logger
{
public:
  Logger()
  {
    _pdata = _data;
  }

  void print(const char* msg)
  {
    _print(msg);
    char *pdata = _pdata;
    _print("---------------\n");
    _pdata = pdata;
  }

  void println(const char* msg)
  {
    print(msg);
    print("\n");
  }
  
  void print(String const& msg)
  {
    print(msg.c_str());
  }

  void println(String const& msg)
  {
    print(msg);
    print("\n");
  }

  void print(int i)
  {
    char buffer[20];
    itoa(i,buffer,10);
    print(buffer);
  }

  void println(int i)
  {
    print(i);
    print("\n"); 
  }

  void print(unsigned long int l)
  {
    char buffer[20];
    ultoa(l,buffer,10);
    print(buffer);
  }

  void println(unsigned long int l)
  {
    print(l);
    print("\n"); 
  }

  void print(float f)
  {
    char buffer[20];
    dtostrf(f, 1, 2, buffer);  // -12.34
    print(buffer);
  }

  void println(float f)
  {
    print(f);
    print("\n");
  }

  const char* getData() const
  {
    return _data;
  }

private:
  void _print(const char* msg)
  {
    for(;;)
    {
      if (_pdata == _data + N - 1)
        _pdata = _data;

      if (*msg == '\0')
        return;

      *_pdata++ = *msg++;
    } 
  }

  char _data[N] = {};
  char *_pdata;
};


#endif
