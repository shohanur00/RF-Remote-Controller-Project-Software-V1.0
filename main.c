#include "stm32f401xc.h"
#include "gpio.h"


int main(void){
	
	GPIO_EnableClock(GPIOC);
	GPIO_Init(GPIOC,13,GPIO_MODE_OUTPUT,GPIO_OTYPE_PP,GPIO_NOPULL,GPIO_SPEED_LOW);
	
	while(1){
			
		GPIO_ResetPin(GPIOC,13);
		
	}
	
}