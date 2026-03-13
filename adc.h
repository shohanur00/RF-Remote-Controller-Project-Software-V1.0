#ifndef ANALOG_READ_H
#define ANALOG_READ_H

#include "stm32f401xc.h"
#include <stdint.h>

// -----------------------------
// Initialize ADC1 peripheral
// Configure resolution, sampling time, etc.
// -----------------------------
void ADC_GPIO_Init(void);
void ADC_Reg_Init(void);
void ADC_DMA_Init(void);
void ADC_Init(void);

// -----------------------------
// Read a single ADC channel (polling mode)
// channel: 0..15 (STM32F4 ADC channels)
// returns 12-bit value (0..4095)
// -----------------------------
uint16_t ADC_Read_Channel(uint8_t channel);

// -----------------------------
// Read multiple channels
// channels: array of channel numbers
// results: array to store ADC readings
// count: number of channels
// -----------------------------
void ADC_Read_Multi(uint8_t *channels, uint16_t *results, uint8_t count);

#endif