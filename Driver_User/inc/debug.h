
/* Define to prevent recursive inclusion ---------------------*/

#ifndef __DEBUG_H
#define __DEBUG_H

/* Includes --------------------------------------------------*/

#include "stm8l15x.h"

/* Private typedef -------------------------------------------*/
/* Private define  -------------------------------------------*/


// If the SEL pin is connected to the voltage supply, the address 
// is 001110b, otherwise the address is 0011110b if the SEL pin is connected to ground


/* Export functions ------------------------------------------*/

void Dbg_putstr(char *msg);
void Dbg_putchr(char data);
void Dbg_sendByte(u16 data);
void Dbg_sendHex(u16 data);
char Dbg_hex2Char(char byHex);
void Dbg_sendInt(u16 data);
void Dbg_sendDword(u32 data);

#endif  /* __DEBUG_H */