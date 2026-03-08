#ifndef I2C1_H
#define I2C1_H

#include <stdint.h>
#include "stm32f401xc.h"

void I2C1_GPIO_Init(void);
void I2C1_Reg_Init(void);
void I2C1_Init(void);

#endif