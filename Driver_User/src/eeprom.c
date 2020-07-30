#include "eeprom.h"
#include "delay.h"
#include "stm8l15x_i2c.h"

uint8_t Byte_Recieve;

void I2C_Config(void){
  CLK_PeripheralClockConfig(CLK_Peripheral_I2C1,ENABLE); 
  I2C_Cmd(I2C1, ENABLE);
  I2C_Init(I2C1,I2C_MAX_STANDARD_FREQ, I2C_SLAVE_ADDRESS, I2C_Mode_I2C, I2C_DutyCycle_2, I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);
}

void WRITER_Byte_To_Eeprom(uint16_t ReadAddr, uint8_t Byte){
  /* While the bus is busy */
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  {}
  
  /* Send Start */
  I2C_GenerateSTART(I2C1, ENABLE);
  /* Test on EV5 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
  {}
  
  /* Send Address */
  I2C_Send7bitAddress(I2C1, I2C_ADDRESS_24E25, I2C_Direction_Transmitter);
  /* Test on EV6 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {}
  
  
  /* Send the EEPROM's internal address to read from: MSB of the address first */
  I2C_SendData(I2C1, (uint8_t)((ReadAddr & 0xFF00) >> 8));
  /* Test on EV8 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {}
  
  /* Send the EEPROM's internal address to read from: LSB of the address */
  I2C_SendData(I2C1, (uint8_t)(ReadAddr & 0x00FF));
  /* Test on EV8 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {}
  
  /* Send Byte */
  I2C_SendData(I2C1, Byte);
  /* Test on EV8 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {}
  
  /*Send Stop*/
  I2C_GenerateSTOP(I2C1, ENABLE);    
  /*Test on EV7 and clear it */
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  {}
}

uint8_t READ_Byte_From_Eeprom(uint16_t ReadAddr){
  
    /* While the bus is busy */
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {}     
    
    /* Send Start */
    I2C_GenerateSTART(I2C1, ENABLE);
    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {}
    
    /* Send Address */
    I2C_Send7bitAddress(I2C1, I2C_ADDRESS_24E25, I2C_Direction_Transmitter);
    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {}
    
    /* Send the EEPROM's internal address to read from: MSB of the address first */
    I2C_SendData(I2C1, (uint8_t)((ReadAddr & 0xFF00) >> 8));
    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {}
    
    /* Send the EEPROM's internal address to read from: LSB of the address */
    I2C_SendData(I2C1, (uint8_t)(ReadAddr & 0x00FF));
    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {}
   
    /* Send STRAT condition a second time */
    I2C_GenerateSTART(I2C1, ENABLE);
    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {}
        
    /* Send Address  and read data */
    I2C_Send7bitAddress(I2C1, I2C_ADDRESS_24E25, I2C_Direction_Receiver);
    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {}
  
    /* Disable Acknowledgement */
    I2C_AcknowledgeConfig(I2C1, DISABLE);
  
    /*Send Stop*/
    I2C_GenerateSTOP(I2C1, ENABLE);
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {}
    
    /* Read a byte from the EEPROM */
    Byte_Recieve = I2C_ReceiveData(I2C1);
    return Byte_Recieve;
}


void WRITER_Frame_To_Eeprom(uint16_t ReadAddr, uint8_t* Frame_Data, uint8_t Size_Of_Frame){
  for(uint8_t i = 0; i < Size_Of_Frame; i++){
    WRITER_Byte_To_Eeprom(ReadAddr, Frame_Data[i]);
    ReadAddr++;
    delay_ms(5);
  }
}


uint8_t READ_Frame_From_Eeprom(uint8_t *Buffer, uint16_t ReadAddr, uint8_t Size_Of_Frame){
  uint8_t Number_Of_Password = 0;
  for(uint8_t i = 0; i < Size_Of_Frame; i++){
    Buffer[i] = READ_Byte_From_Eeprom(ReadAddr);
    if(Buffer[i] == 0xFF)
    {
      Number_Of_Password++;
      if(i == 0||i== 1||i==2)
      return Size_Of_Frame;
    }
    ReadAddr++;
    delay_ms(5);
  }
  return (Size_Of_Frame-Number_Of_Password-3);
}