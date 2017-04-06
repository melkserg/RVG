 #include <Wire.h>
 #include "RvgStats.h"
 #include "BLDC_PWM.h"
 
 
 
 #define EN  4  // enable pin
 static int I2C_adress = 4;
  
  uint8_t g_currentDutyCycle = 255;    // runtime duty cycle
  uint32_t g_passed_ms = 1000;         //time cut off to pwm correction
  uint8_t g_correctionDutyCycleStep = 1; // correction step
  volatile int32_t g_currentPosition = 0;
  uint8_t g_last_position = 1; 
  
  bool STATE1 = true;   // BRAKE
  bool STATE2 = true;   //ACELERATE
  bool STATE3 = true;     // Направление
  
 alternator_recive g_reciveData;
 alternator_send g_sendData; 
 

 void setup() {
 BLDC_INIT();
 I2C_setup();
 Serial.begin(115200);
 pinMode (A0,INPUT);
 pinMode (A1,INPUT);
 pinMode (A2,INPUT);
 pinMode (2,INPUT);
 
//DDRC = 0b00000000;
  attachInterrupt (digitalPinToInterrupt(2),interruptFunction,CHANGE);  //прерывание по положению
  digitalWrite(EN,HIGH);                                             //включение мотора
}

void loop()
{ 
  PWM_SET();
  //PWM_C(true);
  //PWM_SET(analogRead(A3)>>2);
  //BLDC_CYCLE();
  //PWM_SET(analogRead(A3)>>2);
  //PWM_SET(255);
  //Serial.println(PINC & 0b00000111);
  //Serial.println(g_currentPosition);
  g_currentPosition += BLDC_CYCLE (STATE1, STATE2, STATE3);
  //Serial.print("\t output = ");
  //BLDC_CYCLE(PINC & 0b00000111);
  //BLDC_CYCLE();
  
}

void I2C_setup(){

  Wire.begin(I2C_adress);
  Wire.onReceive (receiveEvent);
  Wire.onRequest (requestEvent);
}




void PWM_SET (void)
{ 
  static uint32_t previousMillis = 0; 
         uint32_t currentMillis = millis();
  
  if (currentMillis - previousMillis >= g_passed_ms)
  {
    previousMillis = currentMillis;
     if (g_currentDutyCycle <= g_reciveData.dutyCycle) g_currentDutyCycle += g_correctionDutyCycleStep;
     else g_currentDutyCycle -= g_correctionDutyCycleStep;
  }
  
  OCR0A = g_currentDutyCycle;             /// registers setup
  OCR0B = g_currentDutyCycle;
  OCR1A = g_currentDutyCycle;
  OCR1B = g_currentDutyCycle;
  OCR2A = g_currentDutyCycle;
  OCR2B = g_currentDutyCycle; 
  
  TCCR0B = _BV(CS00);
  TCCR1B =_BV(WGM12)|_BV(CS10);
  TCCR2B = _BV(CS20);
}


void interruptFunction()
{
  g_currentPosition += BLDC_CYCLE (STATE1, STATE2, STATE3);
  //Serial.println(g_currentPosition);
}




void receiveEvent (int howMany)             ////////////////////////////////// прием данных       2С
{ 

  //Serial.println(F("receiveEvent")); 
  
  byte buff[sizeof(g_reciveData)] = {0};
  size_t size = 0;
  while (Wire.available() && (size < sizeof(g_reciveData)))
  {
    buff[size++] = Wire.read();
  }
  
  size_t flushed_bytes = 0;
  while (Wire.available())
  {
    ++flushed_bytes;
  }
  
  if ((size == sizeof(g_reciveData)) && (flushed_bytes == 0))
  {
     g_reciveData = reinterpret_cast <alternator_recive&>(buff);
     STATE1 = g_reciveData.mode&0b00000001;   // BRAKE
     STATE2 = g_reciveData.mode&0b00000010;   //ACELERATE
     STATE3 = g_reciveData.mode&0b00000100;    // MODE
    
  }
  else
  {
    Serial.print(F("alternator_recive error received="));
    Serial.print(size);
    Serial.print(F(" bytes expected="));
    Serial.print(sizeof(alternator_recive));
    Serial.print(F(" flushed="));
    Serial.print(flushed_bytes);
    Serial.print('\n');
  } 
}

void requestEvent ()            ///////////////////////////////////////////////////////// Ответ на запрос  2С
{
  
  //Serial.println(F("Request event"));
  Wire.write ((uint8_t*)& g_sendData, sizeof(g_sendData));
}






