#ifndef __EVS_CALIPER_RF256__H
#define __EVS_CALIPER_RF256__H

#include <arduino.h>
#include <SoftwareSerial.h>

//Идентификация устройства —
//-тип устройства (1)
//-модификация (1)
//-серийный номер (2)
//-зарезервирован (2)
//-диапазон (2)
typedef struct {
  uint8_t type;
  uint8_t modification;
  uint16_t serialNum;
  uint16_t reserved;
  uint16_t range;
} rf256_idn_struct;


class Caliper_RF256 {
  public:
    Caliper_RF256(void);
	
    rf256_idn_struct idn;
    int32_t resultLong;
	float resultFloat;
    int begin(uint8_t addr, HardwareSerial *serial, int txPin, unsigned long speed=9600);
    int begin(uint8_t addr, SoftwareSerial *serial, int txPin, unsigned long speed=9600);
    int doIDN(void);
    int doIDN(rf256_idn_struct &idnStruct);
    char *printIDN(void);
    int32_t doMeas(void);
    float doMeasFloat(void);
  private:
    unsigned char rf256_buffer[20];
	unsigned long rs485_speed=9600;
	bool isHardwareSerial = true;
 	HardwareSerial *rs485_HardwareSerial = &Serial;
	SoftwareSerial *rs485_SoftwareSerial;
	int rs485_txPin=-1;
    uint8_t rf256_addr=-1;
 
    const uint8_t RF256_ADDRMASK =   (uint8_t)0x0F;
    const uint8_t RF256_MSGMASK =    (uint8_t)0x80;
    const uint8_t RF256_PACKETMASK = (uint8_t)0x70;
	
    int makeRequestString(uint8_t request_code, int *numbytes_to_send, int *numbytes_to_receive);
    int doRequest(uint8_t code, uint8_t *recBuf);
    int decodeBuffer(uint8_t *buf, int numBytes, uint8_t *recBuf);
	void sendSerial(uint8_t *buf, int len);
	int receiveSerial(uint8_t *buf, int len);
	int receiveHardwareSerial(uint8_t *buf, int len);
	int receiveSoftwareSerial(uint8_t *buf, int len);
	void setTxMode(bool isTx);
};
#endif // __EVS_CALIPER_RF256__H