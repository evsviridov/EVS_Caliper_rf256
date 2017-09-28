#include "evs_Caliper_rf256.h"

#define RF256_IDN_CMD    ((uint8_t)0x01)
#define RF256_MEAS_CMD   ((uint8_t)0x06)

#define TX_MODE	HIGH
#define RX_MODE LOW

Caliper_RF256::Caliper_RF256(void)
{
  rf256_addr = 0;
}

Caliper_RF256::Caliper_RF256(uint8_t addr, HardwareSerial *serial, int txPin=-1, unsigned long speed=9600)
{
   begin(addr, serial, txPin, speed);
}

int Caliper_RF256::begin(uint8_t addr, HardwareSerial *serial, int txPin, unsigned long speed)
{
  rs485_Serial = serial;
  rs485_speed  = speed;
  rs485_txPin  = txPin;
  
  rs485_Serial->begin(rs485_speed);
  
  if(rs485_txPin != -1) pinMode(rs485_txPin, OUTPUT);
  setTxMode(RX_MODE);
  
  rf256_addr = addr;

}

int Caliper_RF256::doIDN(void)
{
  doRequest(RF256_IDN_CMD, (uint8_t *)&idn);
}

int Caliper_RF256::doIDN(rf256_idn_struct &idnStruct)
{
  doIDN();
  idnStruct = idn;
}

long Caliper_RF256::doMeas(void)
{
  doRequest(RF256_MEAS_CMD, (uint8_t *)&resultLong);
  resultFloat = ((float)resultLong * 0.0001f);
 // memcpy((unsigned char *)&resultLong, rf256_buffer, sizeof(resultLong));
  return resultLong;
}

float Caliper_RF256::doMeasFloat(void)
{
  doMeas();
  return resultFloat;
}

void Caliper_RF256::sendSerial(unsigned char *buf, int len)
{
  setTxMode(TX_MODE);
  rs485_Serial->write(buf, len);
  delayMicroseconds(2000);
 // setTxMode(RX_MODE);
}

int Caliper_RF256::receiveSerial(unsigned char *buf, int len)
{
  setTxMode(RX_MODE);
  for (int i = 0; i < len; ++i)
  {
    int j = 1000;
    do {
      buf[i] = rs485_Serial->read();
      --j;
    } while ((buf[i] == 0xFF) && j);
  }
}

int Caliper_RF256::makeRequestString(uint8_t request_code, int *numbytes_to_send, int *numbytes_to_receive)
{
  switch (request_code)
  {
    case RF256_IDN_CMD :
      *numbytes_to_send = 2;
      *numbytes_to_receive = 8;
      break;
    case RF256_MEAS_CMD :
      *numbytes_to_send = 2;
      *numbytes_to_receive = 4;
      break;
    default :
      *numbytes_to_send = *numbytes_to_receive = -1;
	  return -1;
  }
  //#define RF256_ADDRMASK   (0b00001111)
  //#define RF256_MSGMASK    (0b10000000)
  //#define RF256_PACKETMASK (0b01110000)

  rf256_buffer[0] = RF256_ADDRMASK & rf256_addr;
  rf256_buffer[1] = RF256_MSGMASK | request_code;
  return 0;
}

void Caliper_RF256::setTxMode(bool isTx)
{
	if (rs485_txPin != -1)
		digitalWrite(rs485_txPin, isTx);
}
	

void Caliper_RF256::decodeBuffer(unsigned char *buf, int numBytes, uint8_t *recBuf)
{
  for (int i = 0; i < (numBytes) ; ++i)
  {
    recBuf[i] = ((buf[2*i + 1] & 0x0F) << 4) | (buf[2 * i] & 0x0F);
  }
}

int Caliper_RF256::doRequest(uint8_t code, uint8_t *recBuf)
{
  int numbytes_to_send, numbytes_to_receive;
  makeRequestString(code, &numbytes_to_send, &numbytes_to_receive);
  sendSerial(rf256_buffer, numbytes_to_send);
  receiveSerial(rf256_buffer, numbytes_to_receive * 2);
  decodeBuffer(rf256_buffer, numbytes_to_receive, recBuf);
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

