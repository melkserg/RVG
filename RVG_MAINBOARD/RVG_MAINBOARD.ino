#include <RF24.h>
#include <printf.h>
#include <Time.h>
#include <Wire.h>
#include <SPI.h>
//#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t
#include <avr/pgmspace.h>
#include "TimeRange.h"
#include "RvgRadio.h"
#include "Utils.h"
#include "Config.h"
#include "RvgStats.h"
#include "Proto.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////hx771
const int pins [2] = {5,6};
int tempr = 0;                    //временные данные.
int g_calibrate = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////NRF
RF24 radio(49, 53);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////GLOBAL DATA
//const int angle = 15 ;                 //величина угла крыла в градусах


volatile int g_Vmax = 200;               // максимальная скорость вообще
unsigned int g_AlternatorCorection = 3;  // степень коррекции скорости генератором
//volatile int g_BREAK_LENGTH = 0;       // участок после которого требуется тормозить
//volatile int g_Axeleration = 0;        // ускорение при торможении






bool g_V_STATUS = false;
bool g_BREAK_STATUS = false;
bool breakButton = false;

TimeRange range ; 

const int breakPin1 = 13;// пин для торможения

bool g_reciveEvent = false;
RvgStats_recive g_cmd_code;


Alternator_send            to_send_data;
Alternator_recive          to_recive_data;


volatile unsigned long     g_time          = 0;   // миллисекунды между импульсами
volatile unsigned long     g_length        = 0;
volatile unsigned long     g_power         = 0;
volatile unsigned int      g_power_count   = 0;
volatile unsigned long     g_speed         = 0;   // колличество импульсов за промежуток времени.
volatile unsigned int      g_speed_count   = 0;
volatile long              g_force         = 0;
volatile unsigned int      g_force_count   = 0;

RvgStats                   g_stats  = {1};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////DISPLAY I2C
LiquidCrystal_I2C lcd(0x3F, 20, 4);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////SETUP
void setup() {
  
  PinSetup();
  Wire.begin();                // I2C шина с адресом 4

  Serial.begin(115200);
  while (!Serial) ; ////////////////////////////////////////////////////////////////////// wait until Arduino Serial Monitor opens

  setSyncProvider(RTC.get);   ////////////////////////////////////////////////////////// the function to get the time from the RTC
  if (timeStatus()!= timeSet) Serial.println(F( "Unable to sync with the RTC" ));
  else Serial.println(F( "RTC has set the system time"));                                        

  NrfInit();////////////////////////////////////////////////////////////////////////////////////////////////Включение модуля связи
  //SetTime(Hour,Minute,Second,Day,Month,Year);    ///////////////////////////////////////////////////Только для установки времени
  attachInterrupt(digitalPinToInterrupt(2), NrfReceive, LOW);
  PrintHeaderToSertial();
}


void BreakSetup()
{
}

void PinSetup(void)
{
  pinMode(2, INPUT_PULLUP);
  pinMode(18, INPUT);
}

void loop()////////////////////////////////////////////////////////////////////////////////////////////////////////////////LOOP
{

  if (range.isOver(config::intervalSendMs)) requestI2C();
  
  if (g_reciveEvent)
  {
    makeData();
    sendData();
    WriteAll();
    rule();                                                                                              /// поворот закрылка
    BreakAlternator();                                                                                   /// отправка команды генератору
  }                                                                                         // Добавка функций измерения
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PrintDateToSerial()                          //Выведение в серийный порт данных времени
{
  Serial.print(day());
  Serial.print(' ');
  Serial.print(month());
  Serial.print(' ');
  Serial.print(year());
  Serial.print(' ');

  PrintAlignedTo(Serial, hour());
  Serial.print(':');
  PrintAlignedTo(Serial, minute());
  Serial.print(':');
  PrintAlignedTo(Serial, second());
  Serial.print(' ');
}

void LCD_Display()
{
 // detachInterrupt(digitalPintToInterrupt(20))
  //noInterrupts();
  lcd.setCursor(0,0);
  lcd.print(g_length);
  lcd.print(F("      "));
  //interrupts();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PrintDateToDisplay()                              //Выведение на LCD дисплей данных времени
{
  lcd.clear();
  lcd.setCursor(0, 0);
  PrintAlignedTo(lcd, hour());
  lcd.print(':');
  lcd.setCursor(3, 0);
  PrintAlignedTo(lcd, minute());
  lcd.print(':');
  lcd.setCursor(6, 0);
  PrintAlignedTo(lcd, second());

  lcd.print(F("  "));
  PrintAlignedTo(lcd, day());
  lcd.print(' ');
  PrintAlignedTo(lcd, month());
  lcd.print(' ');
  lcd.print(year());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NrfReceive()                                              //////////Подпрограмма прерывания данных NRF модуля прием команды
{
  //Serial.print(F("I2C"));
  if(radio.available())
  {
    radio.read(&g_cmd_code, sizeof(g_cmd_code));            // сама принимаемая команда
    //breakButton = true;
    g_reciveEvent = true;
  }
  //byte flushByte;
  //while (radio.available()) radio.read (&flushByte, sizeof(flushByte));
  
  //Serial.print(F( "ok" ));
  //Serial.print( g_reciveEvent );      /// Исполняемый умный код  но в прерывании он не нужен
  
  
  to_send_data.dutyCycle = g_cmd_code.dutyCycle;
  to_send_data.mode = g_cmd_code.mode;
  
   
  }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////Запуск модуля связи
void NrfInit()
{
  printf_begin();
  radio.begin();
  delay(2);
  radio.setChannel(RVG_RADIO_CHANNEL);
  radio.setDataRate(RVG_RADIO_RATE);
  radio.setPALevel(RVG_RADIO_POWER);
  radio.openReadingPipe(1, RVG_RADIO_PIPE);
  radio.openWritingPipe(RVG_RADIO_PIPE);
  radio.startListening();
  radio.printDetails();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////Установка времени
void SetTime(int H, int Mi, int S, int D, int Mo, int Y)
{
  tmElements_t tm;
  tm.Hour = H;
  tm.Minute = Mi;
  tm.Second = S;
  tm.Day = D;
  tm.Month = Mo;
  tm.Year = CalendarYrToTm(Y);
  RTC.write(tm);
}




void SendPwm(int a, byte b)///////////////////////////////////////////////////////////////////////////////////////
{
  Wire.beginTransmission(4);
  Wire.write(a > 0 ? 1 : 0);
  Wire.write(b);
  Wire.endTransmission();
}

/////////////////////////////////////////////////////////////////////////////////////////////Выведение всех данных в серийный  порт

void WriteAll()
{
  PrintDateToSerial();
  g_stats.printToSerial();
}


void sendData() ///////////////////////////////////////////////////////////////////////// функция отправки  беспроводная
{ 
  Serial.print(F( "test1\n\r" ));
  noInterrupts();
  radio.stopListening();
  delay(10);
  if (radio.write(&g_stats, sizeof(g_stats))) Serial.print(F( "ok\n\r" ));
  else Serial.print(F( "failed\n\r" ));
  radio.startListening();
  delay(10);
  interrupts();
  g_reciveEvent = false;
}

void makeData(){}
/*{
  noInterrupts();
  g_stats.length = g_length;
  //g_stats.length = 100;
  
  static uint32_t curtime = 0;
  uint16_t interval = millis()- curtime;
  curtime = millis();
  
   g_stats.av_time_speed = g_time;
   g_time = 0;

  if (g_speed_count > 0)
  {
    g_stats.av_speed = g_speed*1000/interval;
    g_stats.speed_count = g_speed_count;
    g_speed = 0;
    g_speed_count = 0;
  }

  if (g_power_count > 0)
  {
    g_stats.av_power = g_power / g_power_count;
    g_stats.power_count = g_power_count;
    g_power = 0;
    g_power_count = 0;
  }

  if (g_force_count > 0)
  {
    g_stats.force = g_force / g_force_count;
    g_force = 0;
    g_stats.force_count = g_force_count;
    g_force_count = 0;
  }

  interrupts();
}
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void hx711Config() // configurtion function to set pins
{
  pinMode(pins[0], OUTPUT);
  digitalWrite(pins[0], LOW);
  pinMode(pins[1], INPUT_PULLUP);
  delay(100);
  if (getData()) g_calibrate = tempr;
}

bool getData()
{
  if (digitalRead(pins[1])== HIGH) return false;
  delay(1);                           // ожидание остальных датчиков для завершения преобразования (при сбоях можно увеличить)
  for (int i = 0 ; i < 16; ++i)       // считываем только первые 16 бит вместо 24 за ненадобностью
  {
    digitalWrite(pins[0], HIGH);       // формирование тактирования
    digitalWrite(pins[0], LOW);
    tempr = (digitalRead(pins[1]) | (tempr << 1)); // Сдвигаем данные и добавляем разряд
  }
  tempr -= g_calibrate;
  for (int i = 0 ; i < 9 + config::adcGain; ++i)
  {
    digitalWrite(pins[0], HIGH);       // формирование тактирования
    digitalWrite(pins[0], LOW);
  }
  g_force += tempr;
  g_force_count++;
  //Serial.print("OK");
  //Serial.println(tempr);
  //Serial.print("OK");
  return true;
}


void rule() ///////////////////////////////////////////////////////////////////////функция поворота паруса
{
   
}


void BreakAlternator()                                                                        // add functionality
{
}









void requestI2C()///////////////////////////////////////////////////////////////////////////////////
{
  Serial.println(F("Starting transmit"));
  
  Wire.beginTransmission (4);
  Wire.write ((uint8_t*)& to_send_data, sizeof(to_send_data));
  Wire.endTransmission();  
  
  Serial.println(F("Starting request"));
  Wire.requestFrom(4,sizeof(to_recive_data));//////////////////////////////////////////////////////////////////////
  if (!Wire.available()) return; 
  byte buff[sizeof(to_recive_data)] = {0};
  size_t size = 0;
  while (Wire.available() && (size < sizeof(to_recive_data)))
  {
    buff[size++] = Wire.read();
  }
  
  size_t flushed_bytes = 0;
  while (Wire.available())
  {
    ++flushed_bytes;
  }
  
  if ((size == sizeof(to_recive_data)) && (flushed_bytes == 0))
  {
    to_recive_data = reinterpret_cast<Alternator_recive&>(buff);
  }
  else
  {
    Serial.print(F("Alternator_recive error received="));
    Serial.print(size);
    Serial.print(F(" bytes expected="));
    Serial.print(sizeof(Alternator_recive));
    Serial.print(F(" flushed="));
    Serial.print(flushed_bytes);
    Serial.print('\n');
  } 
}


