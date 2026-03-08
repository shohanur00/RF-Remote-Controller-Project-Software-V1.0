#include "rcc.h"

/***************************
Crystal: 25MHz
Configure Clock: 84MHz
HSE : 25MHz
LSE	:	32.768KHz
AHB Clock: 84MHz
APB1 Clock: 42 MHz
APB2 Clock: 84 MHz
****************************/

void RCC_HSE_Clock_Config(void){
	
	RCC->CR |= RCC_CR_HSEON;																					//HSE 25 MHz is enabled
	while(!(RCC->CR & RCC_CR_HSERDY)); 																//Wait For HSE ready
	FLASH->ACR  = FLASH_ACR_LATENCY_2WS;															//Flash Latency for high Clock Speed
	
  // PLL config: HSE -> PLL -> 84MHz (USB enabled)
	RCC->PLLCFGR =
			(25U << RCC_PLLCFGR_PLLM_Pos) |      													// M = 25
			(336U << RCC_PLLCFGR_PLLN_Pos) |     													// N = 336
			(1U << RCC_PLLCFGR_PLLP_Pos) |        												// P = 4 (01 means /4 in STM32F4)
			(7U << RCC_PLLCFGR_PLLQ_Pos) |        												// Q = 7 (USB = 48MHz)
			RCC_PLLCFGR_PLLSRC_HSE;              													// HSE source
	
	// Enable PLL
	RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY));
	
	// APB1 prescaler (divide by 2 -> max 42MHz)
  RCC->CFGR &= ~ RCC_CFGR_PPRE1_Msk;
	RCC->CFGR |= (4U << RCC_CFGR_PPRE1_Pos);
	
	// Select PLL as system clock
	RCC->CFGR &= ~RCC_CFGR_SW_Msk;
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	// Wait for switch
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);						// Check that clock switched or not
}


void RCC_LSE_Clock_Config(void){
	
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_DBP;																						//Backup Domain unlock
	
	RCC->BDCR |= RCC_BDCR_LSEON;																			//Enable LSE
	while(!(RCC->BDCR & RCC_BDCR_LSERDY));														//Wait Until Ready
	
	RCC->BDCR &= ~RCC_BDCR_RTCSEL_Msk;
	RCC->BDCR |= ( 1U << RCC_BDCR_RTCSEL_Pos);												//RTC Clock Selection LSE
	RCC->BDCR	|= RCC_BDCR_RTCEN;																			//Enable RTC
	
}


/* Dynamic clock getters */

uint32_t RCC_Get_SYS_CLK(void)
{
    uint32_t sysclk = HSE_VALUE;

    uint32_t src = (RCC->CFGR >> 2) & 0x3;

    switch(src)
    {
        case 0x0: sysclk = 16000000; break;       // HSI
        case 0x1: sysclk = HSE_VALUE; break;       // HSE
        case 0x2:                                       // PLL
        {
            uint32_t pllsrc = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) ?
                              HSE_VALUE : 16000000;

            uint32_t pllm = RCC->PLLCFGR & 0x3F;
            uint32_t plln = (RCC->PLLCFGR >> 6) & 0x1FF;
            uint32_t pllp = ((RCC->PLLCFGR >> 16) & 0x3);
            pllp = (pllp == 0) ? 2 : (pllp == 1) ? 4 :
                   (pllp == 2) ? 6 : 8;

            sysclk = (pllsrc / pllm) * plln / pllp;
            break;
        }
    }

    return sysclk;
}

uint32_t RCC_Get_HCLK(void)
{
    uint32_t hclk = RCC_Get_SYS_CLK();

    static const uint8_t ahb_presc_table[16] =
        {1,1,1,1,2,4,8,16,64,128,256,512,1024,1024,1024,1024};

    uint32_t hpre = (RCC->CFGR >> 4) & 0xF;

    hclk /= ahb_presc_table[hpre];

    return hclk;
}

uint32_t RCC_Get_APB1_Clock(void)
{
    uint32_t pclk = RCC_Get_HCLK();

    static const uint8_t apb_presc_table[8] =
        {1,1,1,1,2,4,8,16};

    uint32_t ppre1 = (RCC->CFGR >> 10) & 0x7;

    pclk /= apb_presc_table[ppre1];

    return pclk;
}

uint32_t RCC_Get_APB2_Clock(void)
{
    uint32_t pclk = RCC_Get_HCLK();

    static const uint8_t apb_presc_table[8] =
        {1,1,1,1,2,4,8,16};

    uint32_t ppre2 = (RCC->CFGR >> 13) & 0x7;

    pclk /= apb_presc_table[ppre2];

    return pclk;
}