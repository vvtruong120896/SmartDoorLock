/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEFAULTDEVICECODE_H
#define __DEFAULTDEVICECODE_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

#define DEFAULT_DEVICE_CODE 1

void REGISTER_DEVICE(uint16_t device_code);

void CONVERSION_Int_To_String(uint16_t device_code);

#endif //__DEFAULTDEVICECODE_H