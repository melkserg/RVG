/*
 * Common radio part for RVG project
 */
#ifndef rvg_radio_h
#define rvg_radio_h

#include <Arduino.h>
#include <RF24.h>
#include <avr/pgmspace.h>

// RVG радио конфигурация
const PROGMEM uint64_t        RVG_RADIO_PIPE    = 0xE8E8F0F0E4ULL; //40 битный виртульный канал связи, выбран случайно
const PROGMEM uint8_t         RVG_RADIO_CHANNEL = 125;
const PROGMEM rf24_datarate_e RVG_RADIO_RATE    = RF24_250KBPS;
const PROGMEM rf24_pa_dbm_e   RVG_RADIO_POWER   = RF24_PA_HIGH;

// RVG радио команды
const PROGMEM uint8_t         RVG_RADIO_BREAK   = 10;

#endif
