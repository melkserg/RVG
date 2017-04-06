#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <avr/pgmspace.h>

enum HX771_GAIN
{
  A_GAIN128,  // 0 - input A;  Gain 128 +- 4mV/V
  B_GAIN64,   // 1 - input B;  Gain 32  +- 16mV/V
  A_GAIN64    // 2 - input A;  Gain 64  +- 8mV/V
};

namespace config
{
  PROGMEM const               HX771_GAIN adcGain   = A_GAIN128;
  
  PROGMEM const int           intervalSendMs       = 100;
  PROGMEM const int           intervalDisplayMs    = 1000;

  PROGMEM const unsigned int  breakForse           = 150;    // сила торможения при нажатии кнопки
  PROGMEM const int           maxLoad              = 60;     // напряжение на лебедке максимальное. (0-255)
  PROGMEM const int           correction           = 20;     // коээфициент коррекции (cтепень приближения к основному положению)
  PROGMEM const int           side                 = -1;     // направление вращения
  PROGMEM const int           angle                = 5;      // угол наклона в относительных единицах (градус = angle*360/128)
  PROGMEM const int           breakAxel            = 1;      // ускорение при торможении
  PROGMEM const unsigned long lineLength           = 50*33;  // длинна всей линии
  PROGMEM const unsigned long axelerationLength    = 15*33;  //длинна ускорения
}

#endif
