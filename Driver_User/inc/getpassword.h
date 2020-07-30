/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GETPASSWORD_H
#define __GETPASSWORD_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/* GET Password */
uint8_t GET_Password(uint8_t* Password_Array);

/* GET Card */
uint8_t GET_Card(uint8_t* Card_Array);

/* GET Key */
uint8_t GET_Key(void);

/* GET Fingerprint */

#endif //__GETPASSWORD_H