/*
 * Radio tranfered stats RVG stucture
 */
#ifndef RvgStats_h
#define RvgStats_h

#include <Arduino.h>

struct RvgStats
{
  volatile uint16_t  av_speed;
  volatile int16_t  av_current;
  volatile uint16_t   av_voltage;
  volatile uint32_t  length;
  volatile int8_t    mode;
  volatile uint8_t  speed_count;
  volatile uint8_t  current_count;
  volatile uint8_t  voltage_count;

  void printToSerial() const;
};

struct RvgStats_recive
{
  volatile uint8_t dutyCycle;
  volatile uint8_t wingAngle;
  volatile uint8_t mode;
};

struct Alternator_send
{
  volatile uint8_t dutyCycle = 254; // 0-254
  volatile uint8_t mode = 0b00000110;/// 010 Acelerate /// 001 Brake /// 100 mode
};

struct Alternator_recive
{
  volatile uint8_t voltage;
  volatile uint8_t vurrent;
  volatile uint8_t position;
};

#endif

