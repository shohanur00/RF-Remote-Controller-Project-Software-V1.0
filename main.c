#include "stm32f401xc.h"
#include "rcc.h"
#include "gpio.h"


int main(void){
	
	RCC_HSE_Clock_Config();
	RCC_LSE_Clock_Config();
	
	GPIO_EnableClock(GPIOC);
	GPIO_Init(GPIOC,13,GPIO_MODE_OUTPUT,GPIO_OTYPE_PP,GPIO_NOPULL,GPIO_SPEED_LOW);
	
	while(1){
			
		GPIO_ResetPin(GPIOC,13);
		
	}
	
}