/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EEPROM_H
#define __EEPROM_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

#define I2C_SLAVE_ADDRESS 0xA0
#define I2C_ADDRESS_24E25 0xA0
#define ADDRESS_KEY_FIRST 0
#define ADDRESS_CARD_FIRST 320
#define ADDRESS_FINGERPRINT_FIRST 640
#define ADDRESS_END 960
#define ADDRESS_DEVICE_CODE_FIRST 1500
#define ADDRESS_DEVICE_CODE_END 1508
#define ALL_BYTE 32768
#define ALL_BYTE_USE 320
#define ALL_FRAME 2048
#define ALL_FRAME_USE 20
#define ALL_FRAME_USE_COMMON 60


//uint8_t ADDRESS_FIRST = 0x00;


void I2C_Config(void);

void WRITER_Byte_To_Eeprom(uint16_t ReadAddr, uint8_t Byte);

uint8_t READ_Byte_From_Eeprom(uint16_t ReadAddr);

void WRITER_Frame_To_Eeprom(uint16_t ReadAddr, uint8_t* Frame_Data, uint8_t Size_Of_Frame);

uint8_t READ_Frame_From_Eeprom(uint8_t *Buffer, uint16_t ReadAddr, uint8_t Size_Of_Frame);

#endif //__EEPROM_H