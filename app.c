#include "stm32f401xc.h"
#include "rcc.h"
#include "gpio.h"
#include "debug.h"
#include "timebase.h"
#include "i2c1.h"
#include "oled.h"
#include "animation.h"
#include "microtimer.h"
#include "robot_eye.h"
#include "adc.h"


int number=0,counter = 660;
uint16_t adc_val[4] = {0};


void gfx_update(void)
{
    if(Oled_Update_Flag_Read())
    {
        Oled_Update_Flag_Clear();   // flag clear

        OLED_Clear();

				OLED_Draw_String(0,0,"Right X:",1);
        OLED_Draw_Number(50,0,adc_val[0],1);
				OLED_Draw_String(0,10,"Right Y:",1);
        OLED_Draw_Number(50,10,adc_val[1],1);
				OLED_Draw_String(0,20,"Left X:",1);
        OLED_Draw_Number(50,20,adc_val[3],1);
				OLED_Draw_String(0,30,"Left Y:",1);
        OLED_Draw_Number(50,30,adc_val[2],1);
				OLED_Update();
    }
}



void adc_read(void){
		adc_val[0] = ADC_Read_Channel(0);
		adc_val[1] = ADC_Read_Channel(1);
		adc_val[2] = ADC_Read_Channel(2);
		adc_val[3] = ADC_Read_Channel(3);
}

void App_Setup(void){
	
	RCC_HSE_Clock_Config();
	RCC_LSE_Clock_Config();
	Timebase_Init(1000);
	Debug_Init(115200);
	//Debug_Tx_Parameter_NL("Runtime:", 0);
	I2C1_Init();
	//Debug_Tx_Parameter_NL("Run Time:", 1);
	OLED_Init(30);
	//Debug_Tx_Parameter_NL("Run Time", 2);
  OLED_Clear();
	//ANIM_Booting(1000);
	OLED_Clear();
  //OLED_Draw_String(0,0,"Hello STM32!");
  MicroTimer_Init();
	ADC_Init();
	//OLED_Update();
	GPIO_EnableClock(GPIOC);
	GPIO_Init(GPIOC,13,GPIO_MODE_OUTPUT,GPIO_OTYPE_PP,GPIO_NOPULL,GPIO_SPEED_LOW);
	//RobotEye_Init();
	Timebase_DownCounter_SS_Set_Securely(0,10);
	Timebase_DownCounter_SS_Set_Securely(1,500);
	Timebase_DownCounter_SS_Set_Securely(2,2000);
	Timebase_DownCounter_SS_Set_Securely(3,1000);
}


void App_Main_Loop(void){

    static uint32_t execution_time = 0;

    if(Timebase_DownCounter_SS_Continuous_Expired_Event(1)){
        
        GPIO_TogglePin(GPIOC,13);
				OLED_Clear();
				//OLED_Draw_String(50,0,"Rmn",1);
				//OLED_Update();

    }
		

		//Debug_Tx_Parameter_NL("ADC Value:",ADC_Read_Channel(1));
		// Measure function execution time
		gfx_update();
		execution_time = MicroTimer_Measure(adc_read);

		Debug_Tx_Parameter_NL("Exc Time:", execution_time);

    Timebase_Main_Loop_Executables();
}