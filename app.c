#include "stm32f401xc.h"
#include "rcc.h"
#include "gpio.h"
#include "debug.h"
#include "timebase.h"
#include "i2c1.h"
#include "oled.h"
#include "animation.h"
#include "microtimer.h"


int number=0,counter = 660;


void my_function(void){
		if(Oled_Update_Flag_Read()){
			
			number++;
			
			Oled_Update_Flag_Clear();
			OLED_Clear();
			
			OLED_Draw_Number(80,0,number,1);
			OLED_Draw_String(0,0,"Hello!",1);
			
			OLED_Update();
			
		}
	
}


void App_Setup(void){
	
	RCC_HSE_Clock_Config();
	RCC_LSE_Clock_Config();
	Timebase_Init(1000);
	Debug_Init(115200);
	Debug_Tx_Parameter_NL("Runtime:", 0);
	I2C1_Init();
	Debug_Tx_Parameter_NL("Run Time:", 1);
	OLED_Init(60);
	Debug_Tx_Parameter_NL("Run Time", 2);
  OLED_Clear();
	ANIM_Booting(1000);
	OLED_Clear();
  //OLED_Draw_String(0,0,"Hello STM32!");
  MicroTimer_Init();
	
	//OLED_Update();
	GPIO_EnableClock(GPIOC);
	GPIO_Init(GPIOC,13,GPIO_MODE_OUTPUT,GPIO_OTYPE_PP,GPIO_NOPULL,GPIO_SPEED_LOW);
	
	Timebase_DownCounter_SS_Set_Securely(0,10);
	Timebase_DownCounter_SS_Set_Securely(1,500);
}


void App_Main_Loop(void){

    static uint32_t execution_time = 0;

    if(Timebase_DownCounter_SS_Continuous_Expired_Event(1)){
        
        GPIO_TogglePin(GPIOC,13);


    }
		
		// Measure function execution time
		execution_time = MicroTimer_Measure(my_function);

		Debug_Tx_Parameter_NL("Exc Time:", execution_time);

    Timebase_Main_Loop_Executables();
}