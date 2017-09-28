#include "evs_Caliper_rf256.h"

Caliper_RF256::Caliper_RF256(void)
{
  rf256_addr = 0;
}

Caliper_RF256::Caliper_RF256(uint8_t addr, int tx_pin)
{
   begin(addr, tx_pin);
}

int Caliper_RF256::begin(uint8_t addr, int tx_pin)
{
  rf256_addr = addr;
  rs485_txPin = tx_pin;
  pinMode(rs485_txPin, OUTPUT);
  digitalWrite(rs485_txPin, LOW);
}

int Caliper_RF256::makeRequestString(uint8_t request_code)
{
  switch (request_code)
  {
    case RF256_IDN_CMD :
      numbytes_to_send = 2;
      numbytes_to_receive = 8;
      break;
    case RF256_MEAS_CMD :
      numbytes_to_send = 2;
      numbytes_to_receive = 4;
      break;
    default :
      return -1;
  }
  //#define RF256_ADDRMASK   (0b00001111)
  //#define RF256_MSGMASK    (0b10000000)
  //#define RF256_PACKETMASK (0b01110000)

  rf256_buffer[0] = RF256_ADDRMASK & rf256_addr;
  rf256_buffer[1] = RF256_MSGMASK | request_code;
  return 0;
}

void Caliper_RF256::sendSerial(unsigned char *buf, int len)
{
  digitalWrite(rs485_txPin, HIGH);
  RS485_Serial.write(buf, len);
  delayMicroseconds(2000);
  digitalWrite(rs485_txPin, LOW);
}

int Caliper_RF256::receiveSerial(unsigned char *buf, int len)
{
  digitalWrite(rs485_txPin, LOW);
  for (int i = 0; i < len; i++)
  {
    int j = 1000;
    do {
      buf[i] = RS485_Serial.read();
      --j;
    } while ((buf[i] == 0xFF) && j);
  }
}

void Caliper_RF256::decodeBuffer(unsigned char *buf, int numBytes)
{
  for (int i = 0; i < (numBytes) ; i++)
  {
    buf[i] = ((buf[2 * i + 1] & 0x0F) << 4) | (buf[2 * i] & 0x0F);
  }
}

int Caliper_RF256::doRequest(uint8_t code)
{
  makeRequestString(code);
  sendSerial(rf256_buffer, numbytes_to_send);
  receiveSerial(rf256_buffer, numbytes_to_receive * 2);
  decodeBuffer(rf256_buffer, numbytes_to_receive);
}

int Caliper_RF256::doIDN(void)
{
  doRequest(RF256_IDN_CMD);
  memcpy((unsigned char *)&idn, rf256_buffer, sizeof(idn));
}

int Caliper_RF256::doIDN(rf256_idn_struct &idnStruct)
{
  doIDN();
  idnStruct = idn;
}

char *Caliper_RF256::printIDN(void)
{
  char str[80];
  //  Serial.print("RF256 address=");
  //  Serial.print(rf256_addr);
  //  Serial.print(", type=");
  //  Serial.print(idn.type);
  //  Serial.print(", modification=");
  //  Serial.print(idn.modification);
  //  Serial.print(", serialNum=");
  //  Serial.print(idn.serialNum);
  //  Serial.print(", range=");
  //  Serial.println(idn.range);
  snprintf(str, sizeof(str)-1, "RF256 address=%d, type=%d, modification=%d, serial=%u, range=%u", rf256_addr, idn.type, idn.modification, idn.serialNum, idn.range);
  Serial.println(str);
  return str;

}

long Caliper_RF256::doMeas(void)
{
  doRequest(RF256_MEAS_CMD);
  memcpy((unsigned char *)&result, rf256_buffer, sizeof(result));
  return result;
}

float Caliper_RF256::doMeasFloat(void)
{
  return (float) ((float)doMeas() * 0.0001f);
}
