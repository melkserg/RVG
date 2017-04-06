#include <Wire.h>
#include <avr/pgmspace.h>
#include "TimeRange.h"
#include "Proto.h"

// PIN конфигурация
const PROGMEM uint8_t POWER_SENSOR1 = A0;
const PROGMEM uint8_t POWER_SENSOR2 = A1;
const PROGMEM uint8_t POWER_SENSOR3 = A2;

const PROGMEM uint8_t RIGHT_PWM_HIGH =  5;
const PROGMEM uint8_t RIGHT_PWM_LOW  =  6;
const PROGMEM uint8_t LEFT_PWM_HIGH  = 10;
const PROGMEM uint8_t LEFT_PWM_LOW   = 11;

const PROGMEM uint8_t INTERUPTION_PIN = 2;
const PROGMEM uint8_t FLAG_PIN = 4;
const PROGMEM uint8_t I2C_ADDRESS = 4;

// Програмные константы
const PROGMEM unsigned long INTERVAL_MS = 10;
const PROGMEM byte          PWM_LIMIT   = 70; // ограничение силы лебедки.

volatile unsigned long g_POWER = 0;
volatile unsigned int g_SPEED = 0;
volatile unsigned int g_TIME = 0;
volatile unsigned int g_COUNTER = 0;

void setup()
{
  pinMode(RIGHT_PWM_HIGH, OUTPUT);
  pinMode(LEFT_PWM_HIGH, OUTPUT);
  pinMode(LEFT_PWM_LOW, OUTPUT);
  pinMode(RIGHT_PWM_LOW, OUTPUT);

  pinMode(INTERUPTION_PIN, INPUT);
  pinMode(FLAG_PIN, OUTPUT);

  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(SendAllToI2C);
  Wire.onReceive(OnI2CReceive);
  Serial.begin(115200);
  Serial.print(F( "ok1" ));
  digitalWrite(FLAG_PIN, LOW);
  attachInterrupt(digitalPinToInterrupt(INTERUPTION_PIN), MeasureSpeed, RISING);
}

void loop()
{
  static TimeRange range;
  //Serial.print(F( "ok2" ));
  MeasurePower();
  if (range.isOver(INTERVAL_MS))          // Вызов прерывания в главной плате.
  {
    Serial.print(F( "ok3" ));
    //SendAllToSerial ();
    digitalWrite(FLAG_PIN, LOW);
    //Wire.begin(I2C_ADDRESS);
    //Wire.onRequest(SendAllToI2C);
    //Wire.onReceive(OnI2CReceive);
  }
}

void SendAllToSerial()
{
  noInterrupts();       // чувствительный к времени код
  unsigned int SPEED = g_SPEED;
  unsigned long POWER = g_POWER;
  unsigned int COUNTER = g_COUNTER;
  g_SPEED = 0;
  g_POWER = 0;
  g_COUNTER = 0;
  interrupts();

  POWER /= COUNTER;     // приводим размерность только для мощности  Serial.print("\t");

  Serial.print(F( "S " ));
  Serial.print(SPEED);
  Serial.print(F( "\t P " ));
  Serial.print(POWER);
  Serial.print(F( "\t C " ));
  Serial.println(COUNTER);
}

void SendAllToI2C()
{
  //noInterrupts();                     // чувствительный к времени код
  unsigned int SPEED = g_SPEED;
  unsigned int TIME = g_TIME;
  unsigned long POWER = g_POWER;
  unsigned int COUNTER = g_COUNTER;
  g_SPEED = 0;
  g_TIME = 0;
  g_POWER = 0;
  g_COUNTER = 0;
  //interrupts();

  POWER /= COUNTER;                // приводим размерность только для мощности и времени
  TIME /= SPEED;                    // Параметр SPEED является счетчиком по своей сути.

  Proto data;
  data.speed = SPEED;
  data.time = TIME;
  data.power = POWER;

  Wire.write((char*)(&data), sizeof(data));
  //Serial.print(sizeof(data));
  digitalWrite(FLAG_PIN, HIGH);
}

void MeasurePower()
{
  unsigned long sensorValue1 = analogRead(POWER_SENSOR1); // диапазон 0-1023 100мкс
  unsigned long sensorValue2 = analogRead(POWER_SENSOR2);
  unsigned long sensorValue3 = analogRead(POWER_SENSOR3);
  
  sensorValue1 *= sensorValue1; // диапазон 0-1046529
  sensorValue2 *= sensorValue2;
  sensorValue3 *= sensorValue3;

  sensorValue1 += sensorValue2 + sensorValue3; // диапазон 3139587

  noInterrupts();
  g_POWER += sensorValue1;
  g_COUNTER++;
  interrupts();
}

void MeasureSpeed()
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  g_TIME += currentMillis - previousMillis  ;///////////////////////////////////////////////////////////напилить правильно нужно
  g_SPEED++;
}

inline byte LimitPwm(byte pwm)
{
	return (pwm <= PWM_LIMIT) ? pwm : PWM_LIMIT;
}

void SetPwm(byte side, byte pwm)
{
  if (side)
  {
    analogWrite(RIGHT_PWM_HIGH, pwm);
    digitalWrite(LEFT_PWM_HIGH, LOW);
    digitalWrite(LEFT_PWM_LOW, HIGH);
    digitalWrite(RIGHT_PWM_LOW, LOW);
  }
  else
  {
    analogWrite(LEFT_PWM_HIGH, pwm);
    digitalWrite(RIGHT_PWM_HIGH, LOW);
    digitalWrite(RIGHT_PWM_LOW, HIGH);
    digitalWrite(LEFT_PWM_LOW, LOW);
  }
  //Serial.println(pwm);
}

void OnI2CReceive(int bytesReady)
{
  byte pwm = 0;
  byte side = 0;
  while (Wire.available()) //???
  {
    side = Wire.read();
    pwm = Wire.read();
  }
  pwm = LimitPwm(pwm);
  SetPwm(side, pwm);
}
