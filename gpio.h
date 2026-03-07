#ifndef GPIO_H
#define GPIO_H

#include "stm32f401xc.h"   // change if using another device

typedef enum {
    GPIO_MODE_INPUT   = 0x00U,
    GPIO_MODE_OUTPUT  = 0x01U,
    GPIO_MODE_AF      = 0x02U,
    GPIO_MODE_ANALOG  = 0x03U
} GPIO_Mode_t;

typedef enum {
    GPIO_OTYPE_PP = 0x00U,
    GPIO_OTYPE_OD = 0x01U
} GPIO_OType_t;

typedef enum {
    GPIO_NOPULL   = 0x00U,
    GPIO_PULLUP   = 0x01U,
    GPIO_PULLDOWN = 0x02U
} GPIO_Pull_t;

typedef enum {
    GPIO_SPEED_LOW      = 0x00U,
    GPIO_SPEED_MEDIUM   = 0x01U,
    GPIO_SPEED_HIGH     = 0x02U,
    GPIO_SPEED_VERYHIGH = 0x03U
} GPIO_Speed_t;


void GPIO_EnableClock(GPIO_TypeDef *GPIOx);
void GPIO_Init(GPIO_TypeDef *GPIOx, uint16_t pin,
               GPIO_Mode_t mode,
               GPIO_OType_t otype,
               GPIO_Pull_t pull,
               GPIO_Speed_t speed);

void GPIO_SetPin(GPIO_TypeDef *GPIOx, uint16_t pin);
void GPIO_ResetPin(GPIO_TypeDef *GPIOx, uint16_t pin);
void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t pin);
uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t pin);

#endif