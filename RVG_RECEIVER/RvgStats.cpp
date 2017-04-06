#include "RvgStats.h"

template <class T>
void static printStat(T stat)
{
  Serial.print(stat);
  Serial.print('\t');
}

void RvgStats::printToSerial() const
{
  Serial.print((millis()));
 
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

void RvgStats_out::printToSerial() const
{

  data.printToSerial();
  printStat(windSpeed);
}
