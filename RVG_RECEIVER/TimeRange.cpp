#include "TimeRange.h"

bool TimeRange::isOver(unsigned long passed_ms)
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= passed_ms)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

