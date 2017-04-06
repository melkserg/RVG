#include "RvgStats.h"

template <class T>
void static printStat(T stat)
{
  Serial.print('\t');
  Serial.print(stat);
}

void RvgStats::printToSerial() const
{
  printStat(av_speed);
  printStat(av_current);
  printStat(av_voltage);
  printStat(length);
  printStat(mode);
  printStat(speed_count);
  printStat(current_count);
  printStat(voltage_count);
  Serial.print('\n');
}
