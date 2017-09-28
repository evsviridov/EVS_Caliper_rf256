#ifndef __EVS_CALIPER_RF256__H
#define __EVS_CALIPER_RF256__H

#include <arduino.h>


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
    Caliper_RF256(uint8_t addr, HardwareSerial *serial, int txPin=-1, unsigned long speed=9600);
	
    rf256_idn_struct idn;
    long resultLong;
	float resultFloat;
    int begin(uint8_t addr, HardwareSerial *serial, int txPin, unsigned long speed=9600);
    int doIDN(void);
    int doIDN(rf256_idn_struct &idnStruct);
    char *printIDN(void);
    long doMeas(void);
    float doMeasFloat(void);
  private:
    unsigned char rf256_buffer[20];
	unsigned long rs485_speed=9600;
 	HardwareSerial *rs485_Serial = &Serial;
	int rs485_txPin=-1;
    uint8_t rf256_addr=-1;
 
    const uint8_t RF256_ADDRMASK = (0b00001111);
    const uint8_t RF256_MSGMASK = (0b10000000);
    const uint8_t RF256_PACKETMASK = (0b01110000);
	
    int makeRequestString(uint8_t request_code, int *numbytes_to_send, int *numbytes_to_receive);
    int doRequest(uint8_t code, uint8_t *recBuf);
    void decodeBuffer(unsigned char *buf, int numBytes, uint8_t *recBuf);
	void Caliper_RF256::sendSerial(unsigned char *buf, int len);
	int Caliper_RF256::receiveSerial(unsigned char *buf, int len);
	void setTxMode(bool isTx);
};
#endif // __EVS_CALIPER_RF256__H