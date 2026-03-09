#include "stm32f401xc.h"
#include "rcc.h"
#include "gpio.h"
#include "debug.h"
#include "timebase.h"
#include "i2c1.h"


uint8_t OLED_Send_Command(uint8_t cmd)
{
    if(I2C1_Start()!=0) return 1;

    if(I2C1_Send_Addr(0x3C,0)!=0) return 2;   // SSD1306 default address

    I2C1_Write_Data(0x00);   // control byte (command)
    I2C1_Write_Data(cmd);

    I2C1_Stop();

    return 0;
}

void OLED_Init(void)
{
    OLED_Send_Command(0xAE); // display OFF
    OLED_Send_Command(0x20); // memory addressing
    OLED_Send_Command(0x10); // page addressing mode

    OLED_Send_Command(0xB0); // page start
    OLED_Send_Command(0xC8); // COM scan direction
    OLED_Send_Command(0x00); 
    OLED_Send_Command(0x10);

    OLED_Send_Command(0x40); // start line
    OLED_Send_Command(0x81); 
    OLED_Send_Command(0xFF); // contrast

    OLED_Send_Command(0xA1); // segment remap
    OLED_Send_Command(0xA6); // normal display

    OLED_Send_Command(0xA8); 
    OLED_Send_Command(0x3F); // multiplex

    OLED_Send_Command(0xA4);
    OLED_Send_Command(0xD3);
    OLED_Send_Command(0x00);

    OLED_Send_Command(0xD5);
    OLED_Send_Command(0xF0);

    OLED_Send_Command(0xD9);
    OLED_Send_Command(0x22);

    OLED_Send_Command(0xDA);
    OLED_Send_Command(0x12);

    OLED_Send_Command(0xDB);
    OLED_Send_Command(0x20);

    OLED_Send_Command(0x8D);
    OLED_Send_Command(0x14);

    OLED_Send_Command(0xAF); // display ON
}



void App_Setup(void){
	
	RCC_HSE_Clock_Config();
	RCC_LSE_Clock_Config();
	Timebase_Init(1000);
	Debug_Init(115200);
	I2C1_Init();
	OLED_Init();
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
			//Debug_Tx_Text_NL("Hello World!");
			//I2C1_Scan_Bus();
			OLED_Init();
		}
		Timebase_Main_Loop_Executables();
	
}