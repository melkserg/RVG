#ifndef PROTO_H
#define PROTO_H

#include <Arduino.h>

struct Proto

{
  uint16_t speed;   // some kind of voltage
  uint16_t time;    // not nessesary
  uint32_t power;    // current
  
};

#endif
