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

  void printTimecode()
  {
    char buffer[20];
    itoa(millis(), buffer, 10);    
    _print(buffer);
    _print(": ");
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
    println(msg.c_str());
  }

  void print(int i)
  {
    char buffer[20];
    itoa(i,buffer,10);
    print(buffer);
  }

  void println(int i)
  {
    char buffer[20];
    itoa(i,buffer,10);
    println(buffer);    
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
