#ifndef MICROTIMER_H
#define MICROTIMER_H

#include "stm32f401xc.h"
#include <stdint.h>
#include "rcc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize TIM2 as a free-running microsecond timer
 * @note 32-bit counter, 1 tick = 1 µs
 */
void MicroTimer_Init(void);

/**
 * @brief Get current TIM2 counter value in microseconds
 * @return uint32_t current timer count
 */
uint32_t MicroTimer_GetCount(void);

/**
 * @brief Measure execution time of a function in microseconds
 * @param func Pointer to the function to measure
 * @return uint32_t execution time in microseconds
 */
uint32_t MicroTimer_Measure(void (*func)(void));

/**
 * @brief Delay for a specified number of microseconds (blocking)
 * @param us Delay in microseconds
 */
void MicroTimer_DelayUs(uint32_t us);

/**
 * @brief Delay for a specified number of milliseconds (blocking)
 * @param ms Delay in milliseconds
 */
void MicroTimer_DelayMs(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif // MICROTIMER_H