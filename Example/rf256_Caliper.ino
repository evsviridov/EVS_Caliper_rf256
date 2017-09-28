#include <evs_Caliper_rf256.h>

#define NUM_OF_CALIPERS 2
uint8_t address[NUM_OF_CALIPERS]={1,2};
Caliper_RF256 rf256[NUM_OF_CALIPERS];

HardwareSerial *p_Serial = &Serial1;
#define RS485_TX_RX_PIN 7

#define PORT_SPEED 9600

void setup()
{
  Serial.begin(9600);

  for (int i = 0; i < NUM_OF_CALIPERS; ++i)
  {
    rf256[i].begin(address[i], p_Serial, RS485_TX_RX_PIN, PORT_SPEED);
    rf256[i].doIDN();
    rf256[i].printIDN();
  }
}

void loop()
{
  static int counter = 0;
  float val;
  Serial.print(++counter); Serial.print(":\t");
  for (int i = 0; i < NUM_OF_CALIPERS; ++i)
  {
    val = rf256[i].doMeasFloat();
    Serial.print((String) "x" + (i + 1) + "=" + String(val, 3) );
    if ( i < (NUM_OF_CALIPERS-1)) Serial.print("\t");
  }
  Serial.println();

  delay(500);
}

