#include <SPI.h>
#include <RF24.h>
#include <printf.h>
#include <avr/pgmspace.h>
#include "RvgRadio.h"
#include "RvgStats.h"
#include "TimeRange.h"

#define GENERATE      ((uint8_t)0b00000001)                      // enables genegating  mode
#define ACELERATE     ((uint8_t)0b00000010)                      // enables acelerating mode
#define ENABLE        ((uint8_t)0b10000000)                      // enables/disables all alternators functions
#define EXTERN        ((uint8_t)0b00000100)                      // enables external windspeed setting

// Radio pins
const PROGMEM uint8_t RADIO_CE = 49; // Chip Enable
const PROGMEM uint8_t RADIO_CS = 53; // Chip Select
const PROGMEM uint8_t RADIO_RD = 2;

// Button pins
const uint8_t BREAK_BUTTON     PROGMEM = 3;                                          // button to break
const uint8_t INTERUPTION_PIN  PROGMEM = 4;

//Sending requests
PROGMEM const int           intervalSendMs       = 100;                              // 10 times per second.

// Serial speed
const long SERIAL_SPEED PROGMEM = 115200L;

// Global variables
RF24 radio(RADIO_CE, RADIO_CS);
volatile bool g_newStats = false;
RvgStats_out g_stats = {0};
RvgStats_send g_send_msg;

volatile uint16_t g_SPEED=0;                                      
volatile uint8_t g_windCoeff= 36;                                  // by using this coefficient wind speed measured in sm/s (10^-2)
volatile bool g_newMeasure = false;                                // teil if newdata available.



void setup() 
{
  Serial.begin(SERIAL_SPEED);
  radioConfig();
  pinsConfig();
}

void loop()
{ 
  static TimeRange range;                                     // sending request every 100 ms.
  if (g_newStats) WriteAll();
  if (range.isOver(intervalSendMs))
  {
    //sendConfig();
    NrfSend();
  }
}

void pinsConfig()//                                                                                 //Pin setup configuration
{
  pinMode(BREAK_BUTTON, INPUT_PULLUP);
  pinMode(INTERUPTION_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BREAK_BUTTON), BreakSend, FALLING);
  //attachInterrupt(digitalPinToInterrupt(BREAK_BUTTON),vindSpeedInterrupt, FALLING);
  //attachInterrupt(digitalPinToInterrupt(INTERUPTION_PIN), vindSpeedInterrupt, RISING);
}

void radioConfig()
{ 
  pinMode(RADIO_RD, INPUT_PULLUP);
  printf_begin();
  radio.begin();
  delay(2);
  radio.setChannel(RVG_RADIO_CHANNEL);
  radio.setDataRate(RVG_RADIO_RATE);
  radio.setPALevel(RVG_RADIO_POWER);
  radio.openWritingPipe(RVG_RADIO_PIPE);
  radio.openReadingPipe(1, RVG_RADIO_PIPE);
  radio.startListening();
  radio.printDetails();
  attachInterrupt(digitalPinToInterrupt(RADIO_RD), NrfReceive, LOW);
}

void sendConfig()                                                                                  //preparing data to send
{
  g_send_msg.dutyCycle=0;
  g_send_msg.wingAngle=10;
  g_send_msg.mode=3;                                           // here must be added some code to solve the speed and position
}


void BreakSend (void)
{
  static TimeRange range2;                                     // sending request every 100 ms.
  if (range2.isOver(200))g_send_msg.dutyCycle += 20;
  //g_send_msg.dutyCycle += 20;
  g_send_msg.wingAngle=0;
  //delay(1000);
  g_send_msg.mode=2;       
}



void NrfSend()                                                                                      // Function to send
{
 noInterrupts();
  radio.stopListening();
  delay(10);
  if (radio.write(&g_send_msg, sizeof(g_send_msg)))
    Serial.print(F(" ok "));
  else
    Serial.print(F(" failed "));
  radio.startListening();
interrupts(); 
}



void NrfReceive()
{
  if (radio.available())
  {
    radio.read(&g_stats, sizeof(g_stats));
    g_newStats = true;
  }
}

void WriteAll()
{
  noInterrupts();
  RvgStats_out temp_stats = g_stats;
  g_newStats = false;
  interrupts();
  temp_stats.printToSerial();                                                            // ≈ 1 мс чтобы переслать
}

void vindSpeedInterrupt()                                                                // Дребезг контактов устранить аппаратно
{
  static uint16_t previousMillis;
  uint16_t currentMillis = millis();
  uint16_t freq = 0;
  
  if(currentMillis - previousMillis<=100) return;
  freq = 10000 / (currentMillis - previousMillis);                                        // frequency measured in DeciHerz (10^-1)
  
  
  //Serial.print (previousMillis);                                                          // debuging commands
  //Serial.print ('\t');
  //Serial.print (previousMillis);  
  //Serial.print ('\t');
  //Serial.print (currentMillis -previousMillis ); 
  //Serial.print ('\t'); 
  //Serial.print (freq);  
  //Serial.print ('\n');
  
  previousMillis = currentMillis;
  g_newMeasure = true;
  g_SPEED = freq * g_windCoeff;
}


