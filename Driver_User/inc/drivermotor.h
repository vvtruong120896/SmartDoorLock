/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DRIVERMOTOR_H
#define __DRIVERMOTOR_H

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/* DrivermotorInit */
void Drivermotor_Init(void);

/* Cho phep mo cua, on motor */
void Drivermotor_Open(void);

/* off motor */
void Drivermotor_Close_All(void);

/* Khong cho phep mo cua, on motor */
void Drivermotor_Close(void);


#endif //__DRIVERMOTOR_H