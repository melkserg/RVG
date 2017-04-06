#ifndef TIMERANGE_H
#define TIMERANGE_H
#include <Arduino.h>

class TimeRange
{
  unsigned long previousMillis = 0;
public:
  bool isOver(unsigned long passed_ms);
};

#endif
