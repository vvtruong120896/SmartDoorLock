#include "defaultdevicecode.h"
#include "define.h"
#include "delay.h"
#include "eeprom.h"
#include "managerpower.h"

uint8_t device_code_buffer[5];

void REGISTER_DEVICE(uint16_t device_code)
{
    WRITER_Byte_To_Eeprom(ADDRESS_DEVICE_CODE_FIRST+0, 'V');
    delay_ms(5);
    WRITER_Byte_To_Eeprom(ADDRESS_DEVICE_CODE_FIRST+1, 'T');
    delay_ms(5);
    WRITER_Byte_To_Eeprom(ADDRESS_DEVICE_CODE_FIRST+2, 'L');
    delay_ms(5);
    WRITER_Byte_To_Eeprom(ADDRESS_DEVICE_CODE_FIRST+3, 'M');
    delay_ms(5);
    CONVERSION_Int_To_String(device_code);
    WRITER_Byte_To_Eeprom(ADDRESS_DEVICE_CODE_FIRST+4, device_code_buffer[0]);
    delay_ms(5);
    WRITER_Byte_To_Eeprom(ADDRESS_DEVICE_CODE_FIRST+5, device_code_buffer[1]);
    delay_ms(5);
    WRITER_Byte_To_Eeprom(ADDRESS_DEVICE_CODE_FIRST+6, device_code_buffer[2]);
    delay_ms(5);
    WRITER_Byte_To_Eeprom(ADDRESS_DEVICE_CODE_FIRST+7, device_code_buffer[3]);
    delay_ms(5);
    WRITER_Byte_To_Eeprom(ADDRESS_DEVICE_CODE_FIRST+8, device_code_buffer[4]);
    delay_ms(5);
}

void CONVERSION_Int_To_String(uint16_t device_code)
{
    uint16_t temp = device_code;
    uint8_t odr1 = 0, odr2 = 0, odr3 = 0, odr4 = 0, odr5 = 0;
    odr5 = temp%10;
    temp = temp/10;
    odr4 = temp%10;
    temp = temp/10;
    odr3 = temp%10;
    temp = temp/10;
    odr2 = temp%10;
    temp = temp/10;
    odr1 = temp;

    device_code_buffer[4] = odr5 + 0x30;
    device_code_buffer[3] = odr4 + 0x30;
    device_code_buffer[2] = odr3 + 0x30;
    device_code_buffer[1] = odr2 + 0x30;
    device_code_buffer[0] = odr1 + 0x30;
}

