#include "stm32f401xc.h"
#include "rcc.h"
#include "gpio.h"
#include "debug.h"
#include "timebase.h"

void App_Setup(void){
	
	RCC_HSE_Clock_Config();
	RCC_LSE_Clock_Config();
	Timebase_Init(1000);
	Debug_Init(115200);
	GPIO_EnableClock(GPIOC);
	GPIO_Init(GPIOC,13,GPIO_MODE_OUTPUT,GPIO_OTYPE_PP,GPIO_NOPULL,GPIO_SPEED_LOW);
	
	Timebase_DownCounter_SS_Set_Securely(0,1000);
	Timebase_DownCounter_SS_Set_Securely(1,500);
}


void App_Main_Loop(void){
	
		if(Timebase_DownCounter_SS_Continuous_Expired_Event(1)){
			GPIO_TogglePin(GPIOC,13);
		}
		
		if(Timebase_DownCounter_SS_Continuous_Expired_Event(0)){
			Debug_Tx_Text_NL("Hello World!");
		}
		Timebase_Main_Loop_Executables();
	
}