// Version 1.0.2

#include "evs_Caliper_rf256.h"

#define RF256_IDN_CMD    ((uint8_t)0x01)
#define RF256_MEAS_CMD   ((uint8_t)0x06)

#define TX_MODE	HIGH
#define RX_MODE LOW

Caliper_RF256::Caliper_RF256(void)
{
  rf256_addr = 0;
}

int Caliper_RF256::begin(uint8_t addr, HardwareSerial *serial, int txPin, unsigned long speed)
{
  isHardwareSerial = true;
  rs485_HardwareSerial = serial;
  rs485_speed  = speed;
  rs485_txPin  = txPin;
  rf256_addr = addr;
  if(rs485_txPin != -1) pinMode(rs485_txPin, OUTPUT);
  setTxMode(RX_MODE);
  
}

#ifdef USE_SOFTWARESERIAL

int Caliper_RF256::begin(uint8_t addr, SoftwareSerial *serial, int txPin, unsigned long speed)
{
  isHardwareSerial = false;
  rs485_SoftwareSerial = serial;
  rs485_speed  = speed;
  rs485_txPin  = txPin;
  rf256_addr = addr;

  if(rs485_txPin != -1)
  {	 
		pinMode(rs485_txPin, OUTPUT);
		setTxMode(RX_MODE);
  }
  
}
#endif

int Caliper_RF256::doIDN(void)
{
  doRequest(RF256_IDN_CMD, (uint8_t *)&idn);
}

int Caliper_RF256::doIDN(rf256_idn_struct &idnStruct)
{
  doIDN();
  idnStruct = idn;
}

int32_t Caliper_RF256::doMeas(void)
{
  if(doRequest(RF256_MEAS_CMD, (uint8_t *)&resultLong) >= 0) 
  {
  resultFloat = ((float)resultLong * 0.0001f);
 // memcpy((unsigned char *)&resultLong, rf256_buffer, sizeof(resultLong));
  }
  return resultLong;
}

float Caliper_RF256::doMeasFloat(void)
{
  doMeas();
  return resultFloat;
}

void Caliper_RF256::sendSerial(uint8_t *buf, int len)
{
  setTxMode(TX_MODE);
#ifdef USE_SOFTWARESERIAL
  if( isHardwareSerial)
  {
	while(rs485_HardwareSerial->available()) rs485_HardwareSerial->read();
	rs485_HardwareSerial->write(buf, len);
  }
  else
  {
	while(rs485_SoftwareSerial->available()) rs485_SoftwareSerial->read();
	rs485_SoftwareSerial->write(buf, len);
  }
#else
	while(rs485_HardwareSerial->available()) rs485_HardwareSerial->read();
	rs485_HardwareSerial->write(buf, len);
#endif

	delayMicroseconds(2000);
//	delay(2);
  setTxMode(RX_MODE);
}

int Caliper_RF256::receiveHardwareSerial(uint8_t *buf, int len)
{
	int i=0;
 	while (i < len)
	{
 		if (rs485_HardwareSerial->available() > 0)
		{
			buf[i++] = rs485_HardwareSerial->read();
		}
	}
}
#ifdef USE_SOFTWARESERIAL
int Caliper_RF256::receiveSoftwareSerial(uint8_t *buf, int len)
{
	int i=0;
	while (i < len)
	{
		if (rs485_SoftwareSerial->available() > 0)
		{
			buf[i++] = rs485_SoftwareSerial->read();
		}
	}
}
#endif

int Caliper_RF256::receiveSerial(uint8_t *buf, int len)
{
	setTxMode(RX_MODE);
	
#ifdef USE_SOFTWARESERIAL
	if( isHardwareSerial )
		receiveHardwareSerial(buf, len);
	else
		receiveSoftwareSerial(buf, len);
#else
		receiveHardwareSerial(buf, len);
#endif	
/*
  for (int i = 0; i < len; ++i)
  {
    uint16_t j = 10000;
    do {
		if( isHardwareSerial)
		{
			buf[i]=rs485_HardwareSerial->read();
		}
		else
		{
			buf[i]=rs485_SoftwareSerial->read();
		}
       --j;
    } while ((buf[i] == 0xFF) && j);
	if ( j == 0 ) buf[i]=(i<<4) | i;
  }
*/ 
}

int Caliper_RF256::makeRequestString(uint8_t request_code, int *numbytes_to_send, int *numbytes_to_receive)
{
  switch (request_code)
  {
    case RF256_IDN_CMD :
      *numbytes_to_send = 2; *numbytes_to_receive = 8;
      break;
    case RF256_MEAS_CMD :
      *numbytes_to_send = 2; *numbytes_to_receive = 4;
      break;
    default :
      *numbytes_to_send = *numbytes_to_receive = -1;
	  return -1;
  }
  //#define RF256_ADDRMASK   (0b00001111)
  //#define RF256_MSGMASK    (0b10000000)
  //#define RF256_PACKETMASK (0b01110000)

  rf256_buffer[0] = RF256_ADDRMASK & rf256_addr;
  rf256_buffer[1] = RF256_MSGMASK  | (request_code & 0x0F);
  return 0;
}

void Caliper_RF256::setTxMode(bool isTx)
{
	if (rs485_txPin != -1)
		digitalWrite(rs485_txPin, isTx);
}

int Caliper_RF256::decodeBuffer(uint8_t *buf, int numBytes, uint8_t *recBuf)
{
	int i;
	uint8_t isOnePacket;
	isOnePacket = buf[0] & RF256_PACKETMASK;
	for(i=1; i<(numBytes*2); ++i)
	{
		if((buf[i] & RF256_PACKETMASK) != isOnePacket)
			return -1;
	}
  for (i = 0; i < (numBytes) ; ++i)
  {
    recBuf[i] = (uint8_t) ((uint8_t)(buf[2*i + 1] & (uint8_t) 0x0F) << 4) | (uint8_t)(buf[2 * i] & (uint8_t)0x0F);
  }
  return 0;
}

int Caliper_RF256::doRequest(uint8_t code, uint8_t *recBuf)
{
  int numbytes_to_send, numbytes_to_receive;
  makeRequestString(code, &numbytes_to_send, &numbytes_to_receive);
  sendSerial(rf256_buffer, numbytes_to_send);
  receiveSerial(rf256_buffer, numbytes_to_receive * 2);
  return decodeBuffer(rf256_buffer, numbytes_to_receive, recBuf);
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

