#include "stm32f401xc.h"
#include "rcc.h"
#include "gpio.h"
#include "debug.h"
#include "timebase.h"
#include "i2c1.h"
#include "oled.h"
#include "animation.h"

int number=0,counter = 660;

void App_Setup(void){
	
	RCC_HSE_Clock_Config();
	RCC_LSE_Clock_Config();
	Timebase_Init(1000);
	Debug_Init(115200);
	I2C1_Init();
	OLED_Init();
  OLED_Clear();
	ANIM_Booting(1000);
	OLED_Clear();
  //OLED_Draw_String(0,0,"Hello STM32!");
  
	
	//OLED_Update();
	GPIO_EnableClock(GPIOC);
	GPIO_Init(GPIOC,13,GPIO_MODE_OUTPUT,GPIO_OTYPE_PP,GPIO_NOPULL,GPIO_SPEED_LOW);
	
	Timebase_DownCounter_SS_Set_Securely(0,10);
	Timebase_DownCounter_SS_Set_Securely(1,200);
}


void App_Main_Loop(void){
	
		if(Timebase_DownCounter_SS_Continuous_Expired_Event(1)){
			GPIO_TogglePin(GPIOC,13);
			
		}
		
		if(Timebase_DownCounter_SS_Continuous_Expired_Event(0)){
			
			OLED_Clear();

			OLED_Update();
			
		}
		Timebase_Main_Loop_Executables();
	
}