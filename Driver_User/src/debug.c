
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

// first include it's own head file
#include "stm8l15x.h"
#include "debug.h"

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/**
 * @func    Dbg_putstr
 * @brief
 * @param
 * @retval  None
 */
void Dbg_putstr(char *msg){
    USART_SendString(USART1, msg);
}

/**
 * @func    Dbg_putchr
 * @brief
 * @param
 * @retval  None
 */
void Dbg_putchr(char data){
    USART_SendData(USART1, data);
}

/**
 * @func    Int2String
 * @brief
 * @param
 * @retval  None
 */
static void Int2String(u16 i, char *s)	// Convert Integer to String
{
    u8 len;
    char *p;
    len = 0;
    p = s;
    do {
        *s = (i % 10) + '0';
        s++;
        len++;
        i /= 10;
    } while (i != 0);
    for (i = 0; i < len / 2; i++) {
        p[len] = p[i];
        p[i] = p[len - 1 - i];
        p[len - 1 - i] = p[len];
    }
    p[len] = 0;
}

/**
 * @func    Int2String
 * @brief
 * @param
 * @retval  None
 */
static void Dword2String(u32 i, char *s)	// Convert Integer to String
{
    u8 len;
    char *p;
    len = 0;
    p = s;
    do {
        *s = (i % 10) + '0';
        s++;
        len++;
        i /= 10;
    } while (i != 0);
    for (i = 0; i < len / 2; i++) {
        p[len] = p[i];
        p[i] = p[len - 1 - i];
        p[len - 1 - i] = p[len];
    }
    p[len] = 0;
}
/**
 * @func    Dbg_sendInt
 * @brief
 * @param
 * @retval  None
 */
void Dbg_sendDword(u32 data){
    char s[11];
    Dword2String(data, &s[0]);
    Dbg_putstr(&s[0]);
}
/**
 * @func    Dbg_sendInt32
 * @brief
 * @param
 * @retval  None
 */
void Dbg_sendInt(u16 data){
    char s[7];
    Int2String(data, &s[0]);
    Dbg_putstr(&s[0]);
}

/**
 * @func   hex2Char
 * @brief  None
 * @param  None
 * @retval
 */
char Dbg_hex2Char(char byHex) {

    char byChar;

    if (byHex < 10) byChar = byHex + 0x30;
    else byChar = byHex + 55;

    return byChar;
}
/**
 * @func    Dbg_sendHex
 * @brief
 * @param
 * @retval  None
 */
void Dbg_sendHex(u16 data){
  
    char str[7];

    Dbg_putstr((char*)"0x");
    str[0] = Dbg_hex2Char((data&0xF000) >> 12);
    str[1] = Dbg_hex2Char((data&0x0F00) >> 8);
    str[2] = Dbg_hex2Char((data&0x00F0) >> 4);
    str[3] = Dbg_hex2Char(data&0x000F);
    str[4] = 0;
    Dbg_putstr((char*)str);
}
/**
 * @func    Dbg_sendHexOneByte
 * @brief
 * @param
 * @retval  None
 */
void Dbg_sendByte(u16 data){

    char str[4];

    str[0] = Dbg_hex2Char((data&0x00F0) >> 4);
    str[1] = Dbg_hex2Char(data&0x000F);
    str[2] = 0;
    Dbg_putstr((char*)str);
}


// End File
