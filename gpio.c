#include "gpio.h"

#include "gpio.h"

void GPIO_EnableClock(GPIO_TypeDef *GPIOx)
{

    if(GPIOx == GPIOA)
    {
        RCC->AHB1ENR |= (1U << 0);
    }
    else if(GPIOx == GPIOB)
    {
        RCC->AHB1ENR |= (1U << 1);
    }
    else if(GPIOx == GPIOC)
    {
        RCC->AHB1ENR |= (1U << 2);
    }
    else if(GPIOx == GPIOD)
    {
        RCC->AHB1ENR |= (1U << 3);
    }
    else if(GPIOx == GPIOE)
    {
        RCC->AHB1ENR |= (1U << 4);
    }
    else if(GPIOx == GPIOH)
    {
        RCC->AHB1ENR |= (1U << 7);
    }

}

void GPIO_Init(GPIO_TypeDef *GPIOx, uint16_t pin,
               GPIO_Mode_t mode,
               GPIO_OType_t otype,
               GPIO_Pull_t pull,
               GPIO_Speed_t speed)
{
    uint32_t pos = pin * 2U;

    /* Mode configuration */
    GPIOx->MODER &= ~(3U << pos);
    GPIOx->MODER |=  (mode << pos);

    /* Output type */
    GPIOx->OTYPER &= ~(1U << pin);
    GPIOx->OTYPER |=  (otype << pin);

    /* Speed */
    GPIOx->OSPEEDR &= ~(3U << pos);
    GPIOx->OSPEEDR |=  (speed << pos);

    /* Pull-up / Pull-down */
    GPIOx->PUPDR &= ~(3U << pos);
    GPIOx->PUPDR |=  (pull << pos);
}

void GPIO_SetPin(GPIO_TypeDef *GPIOx, uint16_t pin)
{
    GPIOx->BSRR = (1U << pin);
}

void GPIO_ResetPin(GPIO_TypeDef *GPIOx, uint16_t pin)
{
    GPIOx->BSRR = (1U << (pin + 16U));
}

void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t pin)
{
    GPIOx->ODR ^= (1U << pin);
}

uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t pin)
{
    return (GPIOx->IDR >> pin) & 0x01U;
}