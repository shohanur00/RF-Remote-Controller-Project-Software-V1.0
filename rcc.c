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