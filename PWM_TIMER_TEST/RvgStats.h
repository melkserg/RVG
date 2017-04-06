/*
 * Radio tranfered stats RVG stucture
 */
#ifndef RvgStats_h
#define RvgStats_h

#include <Arduino.h>


struct alternator_recive
{
  volatile uint8_t dutyCycle = 200;
  volatile uint8_t mode;
};

struct alternator_send
{
  volatile uint8_t voltage;
  volatile uint8_t current;
  volatile uint8_t position;
};

#endif

