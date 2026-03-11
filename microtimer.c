#include "microtimer.h"

void MicroTimer_Init(void)
{
    uint32_t tim_clk = RCC_Get_APB1_Clock();
    uint32_t ppre1 = (RCC->CFGR & RCC_CFGR_PPRE1) >> 10;
    if(ppre1 >= 4) tim_clk *= 2;  // APB1 prescaler >1 -> timer clock x2

    uint32_t prescaler = tim_clk / 1000000 - 1; // 1 tick = 1 us
    if(prescaler > 0xFFFF) prescaler = 0xFFFF;

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->CR1 &= ~TIM_CR1_CEN;  // disable timer
    TIM2->PSC = prescaler;
    TIM2->ARR = 0xFFFFFFFF;     // 32-bit max
    TIM2->EGR = TIM_EGR_UG;     // reload registers
    TIM2->CR1 |= TIM_CR1_CEN;   // start timer
}

uint32_t MicroTimer_GetCount(void)
{
    return TIM2->CNT;
}

uint32_t MicroTimer_Measure(void (*func)(void))
{
    uint32_t start = MicroTimer_GetCount();
    func();
    uint32_t end = MicroTimer_GetCount();

    if(end >= start)
        return end - start;
    else
        return (0xFFFFFFFF - start + end + 1);
}

void MicroTimer_DelayUs(uint32_t us)
{
    uint32_t start = MicroTimer_GetCount();
    while((MicroTimer_GetCount() - start) < us);
}

void MicroTimer_DelayMs(uint32_t ms)
{
    while(ms--)
        MicroTimer_DelayUs(1000);
}