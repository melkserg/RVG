/*
 * Radio tranfered stats RVG stucture
 */
#ifndef RvgStats_h
#define RvgStats_h

#include <Arduino.h>

struct RvgStats
{
  
  volatile uint16_t   av_speed;
  volatile int16_t    av_current;
  volatile uint16_t   av_voltage;
  volatile uint32_t   length;
  volatile int8_t     mode;
  volatile uint8_t    speed_count;
  volatile uint8_t    current_count;
  volatile uint8_t    voltage_count;
  
  void printToSerial() const;
};

struct RvgStats_out
{
  RvgStats data;
  volatile uint16_t windSpeed;
 
  void printToSerial() const;
};

struct RvgStats_send
{
  volatile uint8_t dutyCycle= 0;
  volatile uint8_t wingAngle=0;
  volatile uint8_t mode=0b00000010;
};

#endif
