#ifndef RCC_H
#define RCC_H

#include <stdint.h>
#include "stm32f401xc.h"

/* Crystal definitions */
#define HSE_VALUE 25000000UL
#define LSE_VALUE 32768UL

void RCC_HSE_Clock_Config(void);
void RCC_LSE_Clock_Config(void);

/* Clock functions */
uint32_t RCC_Get_SYSCLK(void);
uint32_t RCC_Get_HCLK(void);
uint32_t RCC_Get_APB1_Clock(void);
uint32_t RCC_Get_APB2_Clock(void);

#endif