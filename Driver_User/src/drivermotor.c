#include "drivermotor.h"
#include "definepin.h"

/* DrivermotorInit */
void Drivermotor_Init(void)
{
  GPIO_Init(GPIOE, GPIO_PIN_MT, GPIO_Mode_Out_PP_Low_Fast);
}

/* Cho phep mo cua, on motor */
void Drivermotor_Open(void)
{
  GPIO_SetBits(GPIOE, GPIO_PIN_MT2);  // Cho phep mo
  GPIO_ResetBits(GPIOE, GPIO_PIN_MT1);
}

/* off motor */
void Drivermotor_Close_All(void)
{
  GPIO_ResetBits(GPIOE, GPIO_PIN_MT1); // Dong lai
  GPIO_ResetBits(GPIOE, GPIO_PIN_MT2);
}

/* Khong cho phep mo cua, on motor */
void Drivermotor_Close(void)
{
  GPIO_ResetBits(GPIOE, GPIO_PIN_MT2); //Khong cho phep mo
  GPIO_SetBits(GPIOE, GPIO_PIN_MT1);
}
