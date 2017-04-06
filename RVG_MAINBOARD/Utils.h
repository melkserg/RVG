#ifndef UTILS_H
#define UTILS_H

template <typename T>
void PrintAlignedTo(T& printer, int digits)                          //доп функция для красивого выведения времени
{
  if (digits < 10)
    printer.print('0');
  printer.print(digits);
}

template <typename T, typename K>
T AbsLimit(T value, K limit) //value - signed value to limit, limit - positive signed value
{
  if (value > limit) value = limit;
  else if (value < -limit) value = -limit;
  return value;
}

inline byte port_C()
{
//  byte c = 0;
//  for (int i = 30; i < 38; ++i)
//  {
//    pinMode(i, INPUT_PULLUP);
//    c = digitalRead(i) | (c << 1);
//  }

  DDRC = 0x00;   // set all port C pins as input
  PORTC = 0xFF;  // set pull-up for all port C pins

  /*flip bits?*/
 
  return PINC;
}

void PrintHeaderToSertial()
{
  Serial.print(F( "TIME/DATA" ));
  Serial.print('\t');
  Serial.print('\t');
  Serial.print(F( "LENGTH" ));
  Serial.print('\t');
  Serial.print(F( "SPEED" ));
  Serial.print('\t');
  Serial.print(F( "POWER" ));
  Serial.print('\t');

  for (int i = 1; i < 10; i++)
   {
     Serial.print(F( "F" ));
     Serial.print(i);
     Serial.print('\t');
   }
  Serial.print('\n');
}

#endif
