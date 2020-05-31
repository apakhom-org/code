// bb    - добавлена индикация в setup() при инициал. GSM
// SMS_3 - периодическая отправка СМС (TimeSMS - период)
//       - в СМС добавлен порядковый номер и признак "Авто"
// SMS_3a  - исправлены ошибки
// SMS_3b  - добавлена обработка высоты (altitude); изменен номер; 15 сек
// SMS_3c1 - добавлены "М" после высоты; вывод порта InPort; вывод напряж.питания; 30 сек
// SMS_3c2 - вывод порта InPort2; при отсутствии спутников - "пустая" СМС 
// SMS_3e  - с2 замена: отсутствие спутников проверка по времени
// SMS_3e1 - тоже, но снова включена передача даты и времени
// SMS_3f  - добавлен вывод числа спутников и отправка СМС по смене состояния входа InPort

//------------------------------------------------------------
// User-defined settings
#define SEND_GPS_TIMESTAMP  //по умолчанию переменная определена
// в СМС посылается информация о времени отсчета координат GPS
//#define SEND_SIM_BALANCE  //по умолчанию переменная определена 
// в СМС посылается информация о балансе
const String MY_NUMBER = "79913190757";
const String BALANCE_CODE = "*100#";
#define TimeSMS    30000      // периодичность автоотправки СМС (мс)
#define TimeSat    3500       // время ожидания спутников перед отпр.СМС
// End of user-defined settings
//------------------------------------------------------------

//------------------------------------------------------------
// Pins and baud rates
#define M590_BAUD      9600
#define GPS_BAUD       9600
// Wake up pin. Wakeup triggered on low
#define M590_RING      2
// Put to sleep pin.
#define M590_DTR       3
#define LED_PIN        13  // в исходном варианте 4
#define InPort         11  // вх порт1 выводимый в СМС  
#define InPort2        10  // вх порт2 выводимый в СМС  
// End of pins and baud rates
//------------------------------------------------------------

//------------------------------------------------------------
// Includes and variables
#include <NMEAGPS.h>
NMEAGPS  gps;
gps_fix  fix;
#define gpsPort Serial
#include <AltSoftSerial.h>
// AltSoftSerial always uses these pins:
//
// Board          Transmit  Receive   PWM Unusable
// -----          --------  -------   ------------
// Arduino Uno        9         8         10
AltSoftSerial m590Serial;
const String MY_NUMBER_WITH_PLUS = "+" + MY_NUMBER;
const String link = "http://maps.google.com/?q="; // <lat>,<lng>
// End of includes and variables
//------------------------------------------------------------
long int var_TimeSMS = 0;     // тут ведем отсчет времени между
                              // автоотправками СМС
boolean AutoSMS = false;      // признак автопосылки СМС
unsigned int NumSMS = 0;      // порядковый номер СМС 
bool typeSMS = true;          // пустая(true), с координатами(false)
bool NewInPort = true;        // прежнее состояние входа InPort
bool Ex = false;              // экстренная СМС по сменене состояния

//------------------------------------------------------------
// Check that the GPS config files are set up properly (NMEAGPS_cfg.h)
#if !defined( NMEAGPS_PARSE_GGA )
#error You must uncomment NMEAGPS_PARSE_GGA in NMEAGPS_cfg.h!
#endif
#if !defined( NMEAGPS_PARSE_GLL )
#error You must uncomment NMEAGPS_PARSE_GLL in NMEAGPS_cfg.h!
#endif
#if defined( NMEAGPS_PARSE_GSA )
#error You must comment NMEAGPS_PARSE_GSA in NMEAGPS_cfg.h!
#endif
#if defined( NMEAGPS_PARSE_GSV )
#error You must comment NMEAGPS_PARSE_GSV in NMEAGPS_cfg.h!
#endif
#if defined( NMEAGPS_PARSE_GST )
#error You must comment NMEAGPS_PARSE_GST in NMEAGPS_cfg.h!
#endif
#if !defined( NMEAGPS_PARSE_RMC )
#error You must uncomment NMEAGPS_PARSE_RMC in NMEAGPS_cfg.h!
#endif
#if defined( NMEAGPS_PARSE_VTG )
#error You must comment NMEAGPS_PARSE_VTG in NMEAGPS_cfg.h!
#endif
#if defined( NMEAGPS_PARSE_ZDA )
#error You must comment NMEAGPS_PARSE_ZDA in NMEAGPS_cfg.h!
#endif
#ifdef NMEAGPS_INTERRUPT_PROCESSING
#error You must comment NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
#endif
#ifdef NMEAGPS_STATS
#error You must comment NMEAGPS_STATS in NMEAGPS_cfg.h!
#endif
#ifndef NMEAGPS_RECOGNIZE_ALL
#error You must define NMEAGPS_RECOGNIZE_ALL in NMEAGPS_cfg.h!
#endif

//------------------------------------------------------------
// Check that the GPS config files are set up properly (GPSfix_cfg.h)
#if !defined( GPS_FIX_DATE )
#error You must uncomment GPS_FIX_DATE in GPSfix_cfg.h!
#endif
#if !defined( GPS_FIX_TIME )
#error You must uncomment GPS_FIX_TIME in GPSfix_cfg.h!
#endif
#if !defined( GPS_FIX_LOCATION )
#error You must uncomment GPS_FIX_LOCATION in GPSfix_cfg.h!
#endif
#if defined( GPS_FIX_LOCATION_DMS )
#error You must comment GPS_FIX_LOCATION_DMS in GPSfix_cfg.h!
#endif
#if !defined( GPS_FIX_ALTITUDE )
#error You must uncomment GPS_FIX_ALTITUDE in GPSfix_cfg.h!
#endif
/* для исходной версии высота не определялась
#if defined( GPS_FIX_ALTITUDE )
#error You must comment GPS_FIX_ALTITUDE in GPSfix_cfg.h!
#endif
*/
#if defined( GPS_FIX_SPEED )
#error You must comment GPS_FIX_SPEED in GPSfix_cfg.h!
#endif
#if defined( GPS_FIX_HEADING )
#error You must comment GPS_FIX_HEADING in GPSfix_cfg.h!
#endif
#if !defined( GPS_FIX_SATELLITES )
#error You must uncomment GPS_FIX_SATELLITES in GPSfix_cfg.h!
#endif
/*#if defined( GPS_FIX_SATELLITES )
#error You must comment GPS_FIX_SATELLITES in GPSfix_cfg.h!
#endif
*/
#if defined( GPS_FIX_HDOP )
#error You must comment GPS_FIX_HDOP in GPSfix_cfg.h!
#endif
#if defined( GPS_FIX_VDOP )
#error You must comment GPS_FIX_VDOP in GPSfix_cfg.h!
#endif
#if defined( GPS_FIX_PDOP )
#error You must comment GPS_FIX_PDOP in GPSfix_cfg.h!
#endif
#if defined( GPS_FIX_LAT_ERR )
#error You must comment GPS_FIX_LAT_ERR in GPSfix_cfg.h!
#endif
#if defined( GPS_FIX_LON_ERR )
#error You must comment GPS_FIX_LON_ERR in GPSfix_cfg.h!
#endif
#if defined( GPS_FIX_ALT_ERR )
#error You must comment GPS_FIX_ALT_ERR in GPSfix_cfg.h!
#endif
#if defined( GPS_FIX_GEOID_HEIGHT )
#error You must comment GPS_FIX_GEOID_HEIGHT in GPSfix_cfg.h!
#endif

void setup() {
//  pinMode(M590_RING, INPUT);
  pinMode(InPort, INPUT);
  digitalWrite(InPort, HIGH);   //подтянем вход
  pinMode(InPort2, INPUT);
  digitalWrite(InPort2, HIGH);   //подтянем вход
//  pinMode(M590_DTR, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  gpsPort.begin(GPS_BAUD);
  m590Serial.begin(M590_BAUD);

  //------------------------------------------------------------
  // M590 GSM modem config
//  digitalWrite(M590_DTR, HIGH);

  digitalWrite(LED_PIN, HIGH);
  newDelay(250);
  digitalWrite(LED_PIN, LOW);

  
  // Basic health check
  sendAndWait("AT", "OK");

  digitalWrite(LED_PIN, HIGH);
  newDelay(250);
  digitalWrite(LED_PIN, LOW);
  newDelay(250);
  digitalWrite(LED_PIN, HIGH);
  newDelay(500);
  digitalWrite(LED_PIN, LOW);



  
  // Get caller's number  Включение режима отображения номера звонящего
  // в строке RING будет присутствовать номер звонящего абонента
  // по умолчанию (AT+CLIP=0) режим отключен - идет просто "RING"
  sendAndWait("AT+CLIP=1", "OK");

  digitalWrite(LED_PIN, HIGH);
  newDelay(250);
  digitalWrite(LED_PIN, LOW);
  newDelay(250);
  digitalWrite(LED_PIN, HIGH);
  newDelay(250);
  digitalWrite(LED_PIN, LOW);
  newDelay(250);
  digitalWrite(LED_PIN, HIGH);
  newDelay(500);
  digitalWrite(LED_PIN, LOW);

  // Set SMS mode to text
  sendAndWait("AT+CMGF=1", "OK");

  // Set character set to GSM (aka ASCII)
  sendAndWait("AT+CSCS=\"GSM\"", "OK");
  // End of M590 GSM modem config
  //------------------------------------------------------------

  digitalWrite(LED_PIN, HIGH);
  newDelay(250);
  digitalWrite(LED_PIN, LOW);
  newDelay(250);
  digitalWrite(LED_PIN, HIGH);
  newDelay(250);
  digitalWrite(LED_PIN, LOW);
  newDelay(250);
  digitalWrite(LED_PIN, HIGH);
  newDelay(250);
  digitalWrite(LED_PIN, LOW);
  newDelay(250);
  digitalWrite(LED_PIN, HIGH);
  newDelay(500);
  digitalWrite(LED_PIN, LOW);

  // Wait for SIM to be registered in local network
  sendAndWait("AT+CREG?", "+CREG: 0,1");

  // One long blink - GSM ready
  digitalWrite(LED_PIN, HIGH);
  newDelay(500);
  digitalWrite(LED_PIN, LOW);

  //------------------------------------------------------------
  // Wait until the fix is available
  while (getFix()) {
    for (int iWait = 0; iWait < 3; iWait ++) {
       digitalWrite(LED_PIN, HIGH);  // индик.ожид.коротк.3 вспышки
       newDelay(50);
       digitalWrite(LED_PIN, LOW);
       newDelay(50);
       }
    };   // ждем спутники

  // Two short blinks - GPS ready
  digitalWrite(LED_PIN, HIGH);
  newDelay(200);
  digitalWrite(LED_PIN, LOW);
  newDelay(100);
  digitalWrite(LED_PIN, HIGH);
  newDelay(200);
  digitalWrite(LED_PIN, LOW);

// отправка контрольной (первой СМС) после включения
  newDelay(1000);
  digitalWrite(LED_PIN, HIGH);

  // Get a fix and write it to global variable
  typeSMS = getFix();
  NewInPort = digitalRead(InPort);

  sendSms(fix, MY_NUMBER_WITH_PLUS);

  digitalWrite(LED_PIN, LOW);
}


void loop() {
//  long int tTek = millis();
  var_TimeSMS = millis();
  
  // Wait for a call (from my number)
  // Т.к. включен режим отображения номера (AT+CLIP=1), то 
  // вместе с "RING" поступает и номер звонящего
  waitFor2Strings("RING", MY_NUMBER);
  //  waitForString("RING");

  // Decline the call  Разорвать ВСЕ соединения ("отбой")
  m590Serial.println("ATH0");

  digitalWrite(LED_PIN, HIGH);

  // Get a fix and write it to global variable
  typeSMS = getFix();

  sendSms(fix, MY_NUMBER_WITH_PLUS);

  digitalWrite(LED_PIN, LOW);
}

long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  
 
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring
 
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
 
  long result = (high<<8) | low;
 
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}

// проверка корректности данных GPS
// false - ОК, спутники есть, данные корректны
// true - спутников нет (время проверки TimeSat)
bool getFix() {
  bool noFix = true;
  long int timeWait = millis() + TimeSat;
  while (noFix && (timeWait > millis()))
  {
    while (gps.available( gpsPort )) {
      fix = gps.read();
      if (fix.valid.location && fix.valid.altitude
#ifdef SEND_GPS_TIMESTAMP
          && fix.valid.date && fix.valid.time
#endif
         ) {
           noFix = false;
           break;
      } 
    }
  }
  return noFix;
}

void sendAndWait(String cmd, String resp) {
  bool gotString = false;
  m590Serial.setTimeout(1000);
  m590Flush();

  while (!gotString) {
    m590Serial.println(cmd);
    gotString = checkString(m590Serial.readString(), resp);
  }
}

// ждем вызов или окончание периода автоотправки СМС
void waitFor2Strings(String in1, String in2) {
  bool gotStrings = false;
  m590Serial.setTimeout(1000);
  String val;

  Ex = false;
  while (!gotStrings) {
    val = m590Serial.readString();
    gotStrings = checkString(val, in1) &&
                 checkString(val, in2);
    if (gotStrings) {AutoSMS = !AutoSMS;}
    else {
      if (AutoSMS) { //проверим - не подошло ли время слать смс по времени
        gotStrings = (var_TimeSMS + TimeSMS) < millis();
        }
      if (!gotStrings) { //проверим смену состояния входа
        gotStrings = digitalRead(InPort) != NewInPort;
        if (gotStrings) {
          NewInPort = !NewInPort;  
          Ex = true;     
          }
        }
      }          
  }
/*
  while (!gotStrings && (var_TimeSMS + TimeSMS) > millis()) {
    val = m590Serial.readString();
    gotStrings = checkString(val, in1) &&
                 checkString(val, in2);
  }
*/
}

void waitForString(String in) {
  bool gotString = false;
  m590Serial.setTimeout(1000);
  m590Flush();

  while (!gotString) {
    gotString = checkString(m590Serial.readString(), in);
  }
}

bool checkString(String val, String in) {
  return (val.indexOf(in) > -1);
}

void sendSms(gps_fix & fix, String number)
{
  NumSMS ++;     // порядковый номер СМС
  
  m590Flush();   // принимаем все, что идет с GSM
#ifdef SEND_SIM_BALANCE
  String b = getBalance();
#endif

  // Wait for prompt >
  sendAndWait("AT+CMGS=\"" + number + "\"", ">");

  m590Serial.print(NumSMS);                 // порядковый номер
  m590Serial.print(' ');
  if (Ex) {m590Serial.print('C');}          // признак "экстренной" СМС
  else {
    if (AutoSMS) {m590Serial.print('A');}   // автоотправка СМС
    else {m590Serial.print('S');}           // стоп автоотправки СМС
    }
  m590Serial.print(' ');

  if (!typeSMS) {
    m590Serial.print(fix.altitude(), 1);    // высота
    m590Serial.print('M');                  // в метрах
    m590Serial.print(' ');
    }
  
  if (!typeSMS) {
    float _sat = (float) fix.satellites * 1;
    m590Serial.print(_sat, 0);              // число спутников
    m590Serial.print('S');                  // в satellites
    m590Serial.print(' ');
    }
  
  if (NewInPort) {
    m590Serial.print('H');}               // на входе "1"
  else {m590Serial.print('L');}           // "0"
  m590Serial.print(' ');

  if (digitalRead(InPort2) == HIGH) {
    m590Serial.print('H');}               // на входе "1"
  else {m590Serial.print('L');}           // "0"
  m590Serial.print(' ');

  float VCC = (float)readVcc()/1000.0;
  m590Serial.print(VCC, 2);               // напряжение питания
  m590Serial.print('V');                  // в вольтах
  m590Serial.print(' ');
  
  if (!typeSMS) {
#ifdef SEND_GPS_TIMESTAMP
  m590Serial.print(fix.dateTime.year);
  m590Serial.print('-');
  m590Serial.print(fix.dateTime.month);
  m590Serial.print('-');
  m590Serial.print(fix.dateTime.date);
  m590Serial.print(',');
  m590Serial.print(fix.dateTime.hours);
  m590Serial.print(':');
  m590Serial.print(fix.dateTime.minutes);
  m590Serial.print(':');
  m590Serial.print(fix.dateTime.seconds);
  sendAndWait(" UTC ", ">");
#endif

  // Send maps.google.com/blabla
  m590Serial.print(link);

  // Lat + lon from fix
  m590Serial.print( fix.latitude(), 6 );  //56.0000
  m590Serial.print( ',' );
  m590Serial.print( fix.longitude(), 6 ); //46.0000
  }

#ifdef SEND_SIM_BALANCE
  sendAndWait("", ">");
  m590Serial.print("Balance is ");
  m590Serial.print(b);
  m590Serial.print(" RUR.");
#endif

  // End character
  m590Serial.print((char)0x1A);
}

String getBalance() {
  m590Flush();
  // Get current balance. This can take a while, so I set timeout to 10s
  String bal = sendAndGetResp("ATD" + BALANCE_CODE, "OK", 10);
  int startMes  = bal.indexOf('\"');
  int endMes  = bal.lastIndexOf('\"');

  // Crop response so that only message is left
  bal =  bal.substring(startMes + 1, endMes);

  int firstDigit = -1;
  int lastDigit = -1;
  for (int i = 0; i < bal.length(); i++) {
    if (firstDigit == -1) { // locate first digit in message
      if (isDigit(bal.charAt(i))) {
        firstDigit = i;
      }
    } else { // locate last digit in message
      if (!(isDigit(bal.charAt(i)) ||
            (bal.charAt(i) == ',') ||
            (bal.charAt(i) == '.'))) {
        lastDigit = i;
        break;
      }
    }
  }
  // Crop the message to contain only float value
  bal = bal.substring(firstDigit, lastDigit);
  return bal;
}

String sendAndGetResp(String cmd, String resp, int tries) {
  bool gotString = false;
  String rawResponse = "";
  m590Serial.setTimeout(1000);
  m590Flush();

  while (!gotString) {
    m590Serial.println(cmd);
    for (int i = 0; i < tries; i++) {
      rawResponse += m590Serial.readString();
      gotString = checkString(rawResponse, resp);
      if (gotString) {
        break;
      }
    }
  }
  if (!gotString) {
    rawResponse = "\"Balance is 999.999\"";
  }
  return rawResponse;
}

void newDelay(int ms) {
  m590Serial.setTimeout(ms);
  m590Serial.readString();
}

void m590Flush() {
  while (m590Serial.available()) {
    m590Serial.read();
  }
}
