#ifndef __EVS_CALIPER_RF256__H
#define __EVS_CALIPER_RF256__H

#include <arduino.h>

#define RS485_Serial Serial1

#define RS485_PORT_SPEED 9600

#define RF256_IDN_CMD    ((uint8_t)0x01)
#define RF256_MEAS_CMD   ((uint8_t)0x06)

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
    rf256_idn_struct idn;
    long result;
    Caliper_RF256(void);
    Caliper_RF256(uint8_t addr, int txPin=-1);
    int begin(uint8_t addr);
    int begin(uint8_t addr, int txPin);
    int doIDN(void);
    int doIDN(rf256_idn_struct &idnStruct);
    char *printIDN(void);
    long doMeas(void);
    float doMeasFloat(void);
  private:
    const uint8_t RF256_ADDRMASK = (0b00001111);
    const uint8_t RF256_MSGMASK = (0b10000000);
    const uint8_t RF256_PACKETMASK = (0b01110000);

    uint8_t rf256_addr=-1;
    unsigned char rf256_buffer[20];
	int rs485_txPin=-1;
    int numbytes_to_send, numbytes_to_receive;
    int makeRequestString(uint8_t request_code);
    int makeRequestString(uint8_t request_code, uint8_t param, int numBytes);
    int doRequest(uint8_t code);
    void decodeBuffer(unsigned char *buf, int numBytes);
	void Caliper_RF256::sendSerial(unsigned char *buf, int len);
	int Caliper_RF256::receiveSerial(unsigned char *buf, int len);

};
#endif // __EVS_CALIPER_RF256__H