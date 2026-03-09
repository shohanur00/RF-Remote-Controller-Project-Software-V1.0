#include "stm32f401xc.h"
#include "rcc.h"
#include "gpio.h"
#include "debug.h"
#include "timebase.h"
#include "i2c1.h"
#include "oled.h"


int number=0,counter = 660;

void App_Setup(void){
	
	RCC_HSE_Clock_Config();
	RCC_LSE_Clock_Config();
	Timebase_Init(1000);
	Debug_Init(115200);
	I2C1_Init();
	OLED_Init();
  OLED_Clear();
  //OLED_Draw_String(0,0,"Hello STM32!");
  
	
	OLED_Update();
	GPIO_EnableClock(GPIOC);
	GPIO_Init(GPIOC,13,GPIO_MODE_OUTPUT,GPIO_OTYPE_PP,GPIO_NOPULL,GPIO_SPEED_LOW);
	
	Timebase_DownCounter_SS_Set_Securely(0,100);
	Timebase_DownCounter_SS_Set_Securely(1,5000);
}


void App_Main_Loop(void){
	
		if(Timebase_DownCounter_SS_Continuous_Expired_Event(1)){
			GPIO_TogglePin(GPIOC,13);
			counter = 0;
			//OLED_Draw_Circle(20,20,10);
			OLED_Update_Partial(10,1,30,4);
		}
		
		if(Timebase_DownCounter_SS_Continuous_Expired_Event(0)){
			
			//Debug_Tx_Text_NL("Hello World!");
			//I2C1_Scan_Bus();
			//OLED_Init();
			//OLED_Update();
			//OLED_Draw_String(0,0,"Osama!");
			//char buf[5];
			//sprintf(buf,"%d",counter);

			// Draw number at x=60, page=3
			OLED_Draw_Int(60, 3, counter,3);

			// Update only this small area
			OLED_Update_Partial(60, 3, 90, 3); 

			counter++;
			//delay_ms(500);
		}
		Timebase_Main_Loop_Executables();
	
}