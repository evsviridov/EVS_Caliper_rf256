#include <evs_Caliper_rf256.h>

#define RS485_TX_RX_PIN 7

#define RS485_PORT_SPEED 9600

Caliper_RF256 rf256[2];

int counter = 0;

void setup()
{
  Serial.begin(9600);
  Serial1.begin(RS485_PORT_SPEED);

  rf256[0].begin(1, RS485_TX_RX_PIN);
  rf256[1].begin(2, RS485_TX_RX_PIN);
  for (int i = 0; i < 2; ++i)
  {
    rf256[i].doIDN();
    rf256[i].printIDN();
  }
}

void loop()
{
  float val;
  Serial.print(++counter);
  Serial.print(":\t");
  for (int i = 0; i < 2; ++i)
  {
    //    val = rf256[i].doMeas() * 0.0001f;
    val = rf256[i].doMeasFloat();
    Serial.print((String) "x" + (i + 1) + "=" + String(val, 3) );
    if ( i != 1) Serial.print("\t");
  }
  Serial.println();


  delay(1000);
}

