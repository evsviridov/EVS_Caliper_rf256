#include <evs_Caliper_rf256.h>

//#define DO_IDN

#define NUM_OF_CALIPERS 2
uint8_t address[NUM_OF_CALIPERS] = {1, 2};
Caliper_RF256 rf256[NUM_OF_CALIPERS];

//#define SOFT_SERIAL

#ifndef SOFT_SERIAL
#define HARD_SERIAL
#endif

#ifdef HARD_SERIAL

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
HardwareSerial *p_HardSerial = &Serial3;
#else
#error "Only MEGA board support Hardware Serial"
#endif // ... __AVR_ATmega2560__

#endif // HARD_SERIAL

#ifdef SOFT_SERIAL
#define rxPin 15
#define txPin 14
SoftwareSerial SoftSerial(rxPin, txPin);
SoftwareSerial *p_SoftSerial = &SoftSerial;
#endif // SOFT_SERIAL

#define RS485_TX_RX_PIN 22

#define PORT_SPEED 9600

void setup()
{
  Serial.begin(9600);

  for (int i = 0; i < NUM_OF_CALIPERS; ++i)
  {
#ifdef HARD_SERIAL
    rf256[i].begin(address[i], p_HardSerial, RS485_TX_RX_PIN, PORT_SPEED);
#endif // HARD_SERIAL
#ifdef SOFT_SERIAL
    rf256[i].begin(address[i], p_SoftSerial, RS485_TX_RX_PIN, PORT_SPEED);
#endif // SOFT_SERIAL
#ifdef DO_IDN
    rf256[i].doIDN();
    rf256[i].printIDN();
#endif // DO_IDN
  }
}

void loop()
{
  static int counter = 0;
  float valFloat;
  int32_t valLong;
  //  Serial.print(++counter); Serial.print(":\t");
  for (int i = 0; i < NUM_OF_CALIPERS; ++i)
  {
    rf256[i].doMeas();
    valLong = rf256[i].resultLong;
    valFloat = rf256[i].resultFloat;
    //    Serial.print((String) "x" + (i + 1) + "=" + String(valFloat, 3) ); // + "  x_long=" + valLong );
    Serial.print(valFloat, 4); // + "  x_long=" + valLong );
    if ( i < (NUM_OF_CALIPERS - 1)) Serial.print("\t");
  }
  Serial.println();

  delay(250);
}