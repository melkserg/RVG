#include "BLDC_PWM.h"


void BLDC_INIT()
{
pinMode (AH,OUTPUT);   //OC2B
 pinMode (AL, OUTPUT); //OC2A
 pinMode (BH,OUTPUT);   //OC0B
 pinMode (BL, OUTPUT);  //OC0A
 pinMode (CH,OUTPUT);  //OC1B 
 pinMode (CL, OUTPUT); //OC1A
 pinMode (EN, OUTPUT);
}


int8_t BLDC_CYCLE( bool STATE1, bool STATE2, bool STATE3)    // state 1 brake // state 2 drive // state 3 way
{
  static uint8_t g_last_position = 1;                        /// last position
  int delta = 0;                                             /// tmp data to find position
  delay (10);
  uint8_t reverse_states [] {0,1,3,2,5,0,4};
  int cycle = reverse_states [(PINC & 0b00000111)];
  
  
  if (STATE3) cycle += 3 ;
  if (cycle > 5) cycle -= 6;
  
  switch (cycle){
   
    case 0 :
          digitalWrite(CL,HIGH);
          digitalWrite(CH,HIGH);
          digitalWrite(BH,HIGH);
          if(STATE2) TCCR0A =  _BV(COM0B0)|_BV(WGM01)| _BV(WGM00);
          if(STATE2) TCCR0A |= _BV(COM0A1);
          if(STATE1) TCCR0A |= _BV(COM0B1);  
          digitalWrite(BL,LOW);
          if (g_last_position == 6) delta = 1;
          if (g_last_position == 3) delta = -1;
          g_last_position = 5;
          break ;
          
    case 1 :
          digitalWrite(BL,HIGH);
          digitalWrite(CH,HIGH);
          digitalWrite(BH,HIGH);
          if(STATE2) TCCR0A = _BV(COM0B0)|_BV(WGM01)| _BV(WGM00);
          if(STATE2) TCCR0A |= _BV(COM0A1);
          if(STATE1) TCCR0A |= _BV(COM0B1);
          digitalWrite(CL,LOW);
          break ;

    case 2 :
          digitalWrite(AL,HIGH);
          digitalWrite(CH,HIGH);
          digitalWrite(AH,HIGH);
          if(STATE2) TCCR1A =_BV(COM1B0)|_BV(WGM10);
          if(STATE2) TCCR1A |= _BV(COM1A1);
          if(STATE1) TCCR1A |= _BV(COM1B1);
          digitalWrite(CL,LOW);
          if (g_last_position == 5) delta = 1;
          if (g_last_position == 6) delta = -1;
          g_last_position = 3;
          break ;
          
    case 3 :
          digitalWrite(CL,HIGH);
          digitalWrite(CH,HIGH);
          digitalWrite(AH,HIGH);          
          if(STATE2) TCCR1A = _BV(COM1B0)|_BV(WGM10);
          if(STATE2) TCCR1A |= _BV(COM1A1);
          if(STATE1) TCCR1A |= _BV(COM1B1); 
          digitalWrite(AL,LOW);
          break ;
          
    case 4 :
          digitalWrite(BL,HIGH);
          digitalWrite(BH,HIGH);
          digitalWrite(AH,HIGH);  
          if(STATE2) TCCR2A = _BV(COM2B0)|_BV(WGM21)|_BV(WGM20);
          if(STATE2) TCCR2A |= _BV(COM2A1);
          if(STATE1) TCCR2A |= _BV(COM2B1);
          digitalWrite(AL,LOW);
          if (g_last_position == 3) delta = 1;
          if (g_last_position == 5) delta = -1;
          g_last_position = 6;
          break ;
    
    case 5 :
          digitalWrite(AL,HIGH);
          digitalWrite(BH,HIGH);
          digitalWrite(AH,HIGH);
          if(STATE2) TCCR2A =_BV(COM2B0)|_BV(WGM21)|_BV(WGM20);
          if(STATE2) TCCR2A |= _BV(COM2A1);
          if(STATE1) TCCR2A |= _BV(COM2B1);  
          digitalWrite(BL,LOW);
          break ;
          }
          //Serial.println(delta);
 return delta;
}

/* void PWM_A(bool STATE1)
{  
  TCCR0B =_BV(CS00) |_BV(CS02);
  if(STATE1)TCCR0A = _BV(COM0A1)|_BV(COM0B1)| _BV(COM0B0)|_BV(WGM01)| _BV(WGM00); 
  else TCCR0A &=  ~(_BV(COM0A1)|_BV(COM0B1));
}

void PWM_B(bool STATE1)
{ 
  TCCR1B =_BV(WGM12)|_BV(CS10) |_BV(CS12);
  if(STATE1)TCCR1A = _BV(COM1A1)|_BV(COM1B1)| _BV(COM1B0)|_BV(WGM10); 
  else TCCR1A =  ~(_BV(COM1A1)|_BV(COM1B1));
}

void PWM_C(bool STATE1)
{ 
  TCCR2B = _BV(CS20)|_BV(CS21)|_BV(CS22);  
  if(STATE1)TCCR2A = _BV(COM2A1)|_BV(COM2B1)|_BV(COM2B0)|_BV(WGM21)|_BV(WGM20);     
  else TCCR2A =  ~(_BV(COM2A1)|_BV(COM2B1));
}


int8_t BLDC_CYCLE( bool STATE1, bool STATE2, bool STATE3)
{
  static uint8_t g_last_position = 1;
  int delta = 0;
  delay (5);
  int cycle = (PINC & 0b00000111);
  
  //Serial.println(cycle);
 
  switch (cycle){
   
    //case 2 :
    //case 6 :
    //case 4 :
    case 5 :
    //case 1 :
    //case 3 :
    
          digitalWrite(CL,HIGH);
          digitalWrite(CH,HIGH);
          digitalWrite(BH,HIGH);
          //TCCR0B = _BV(CS01);
          if(STATE2) TCCR0A =  _BV(COM0B0)|_BV(WGM01)| _BV(WGM00);
          //if(STATE1)digitalWrite(AL,HIGH);
          //if(STATE2)digitalWrite(AH,HIGH);
          if(STATE2) TCCR0A |= _BV(COM0A1);
          if(STATE1) TCCR0A |= _BV(COM0B1);  
          digitalWrite(BL,LOW);
          if (g_last_position == 6) delta = 1;
          if (g_last_position == 3) delta = -1;
          g_last_position = 5;
          break ;
          
    //case 6 :
    //case 4 :
    //case 5 :
    case 1 :
    //case 3 :
    //case 2 :
     
          digitalWrite(BL,HIGH);
          digitalWrite(CH,HIGH);
          digitalWrite(BH,HIGH);
          //TCCR0B = _BV(CS01);
          if(STATE2) TCCR0A = _BV(COM0B0)|_BV(WGM01)| _BV(WGM00);
          //if(STATE1)digitalWrite(AL,HIGH);
          //if(STATE2)digitalWrite(AH,HIGH);
          if(STATE2) TCCR0A |= _BV(COM0A1);
          if(STATE1) TCCR0A |= _BV(COM0B1);
          digitalWrite(CL,LOW);
          break ;
          
    //case 4 :
    //case 5 :
    //case 1 :
    case 3 :
    //case 2 :
    //case 6 :
    
          digitalWrite(AL,HIGH);
          digitalWrite(CH,HIGH);
          digitalWrite(AH,HIGH);
          //TCCR1B =_BV(WGM12)|_BV(CS11);              ///  ////////////////////////////////OK
          if(STATE2) TCCR1A =_BV(COM1B0)|_BV(WGM10);
          //if(STATE1)digitalWrite(BL,HIGH);
          //if(STATE2)digitalWrite(BH,HIGH);
          if(STATE2) TCCR1A |= _BV(COM1A1);
          if(STATE1) TCCR1A |= _BV(COM1B1);
          digitalWrite(CL,LOW);
          if (g_last_position == 5) delta = 1;
          if (g_last_position == 6) delta = -1;
          g_last_position = 3;
          break ;
          
    //case 5 :
    //case 1 :
    //case 3 :
    case 2 :
    //case 6 :
    //case 4 :
     
          digitalWrite(CL,HIGH);
          digitalWrite(CH,HIGH);
          digitalWrite(AH,HIGH);
          //TCCR1B =_BV(WGM12)|_BV(CS11);                  //////////////////////////////////OK
          if(STATE2) TCCR1A = _BV(COM1B0)|_BV(WGM10);
          //if(STATE1)digitalWrite(BL,HIGH);
          //if(STATE2)digitalWrite(BH,HIGH);
          if(STATE2) TCCR1A |= _BV(COM1A1);
          if(STATE1) TCCR1A |= _BV(COM1B1); 
          digitalWrite(AL,LOW);
          break ;
          
    //case 1 :
    //case 3 :
    //case 2 :
    case 6 :
    //case 4 :
    //case 5 :
          
          digitalWrite(BL,HIGH);
          digitalWrite(BH,HIGH);
          digitalWrite(AH,HIGH);
          //TCCR2B = _BV(CS21);  
          if(STATE2) TCCR2A = _BV(COM2B0)|_BV(WGM21)|_BV(WGM20);
          //if(STATE1)digitalWrite(CL,HIGH);
          //if(STATE2)digitalWrite(CH,HIGH);
          if(STATE2) TCCR2A |= _BV(COM2A1);
          if(STATE1) TCCR2A |= _BV(COM2B1);
          digitalWrite(AL,LOW);
          if (g_last_position == 3) delta = 1;
          if (g_last_position == 5) delta = -1;
          g_last_position = 6;
          break ;
    //case 3 :
    //case 2 :
    //case 6 :
    case 4 :
    //case 5 :
    //case 1 :
     
          digitalWrite(AL,HIGH);
          digitalWrite(BH,HIGH);
          digitalWrite(AH,HIGH);
          //TCCR2B = _BV(CS21);  
          if(STATE2) TCCR2A =_BV(COM2B0)|_BV(WGM21)|_BV(WGM20);
          //if(STATE1)digitalWrite(CL,HIGH);
          //if(STATE2)digitalWrite(CH,HIGH);
          if(STATE2) TCCR2A |= _BV(COM2A1);
          if(STATE1) TCCR2A |= _BV(COM2B1);  
          digitalWrite(BL,LOW);
          //g_last_position=0
          
          break ;
          }
 return delta;
}

*/
