#ifndef OLED_H
#define OLED_H

#include <stdint.h>
#include <stdio.h>
#include "i2c1.h"
#include "rcc.h"

//------------------ color ------------------
#define OLED_BLACK 0
#define OLED_WHITE 1
#define OLED_INVERT 2  // optional, future use if needed

//------------------ OLED size (SSD1306 128x64) ------------------
#define OLED_WIDTH        128
#define OLED_HEIGHT       64
#define OLED_BUFFER_SIZE  (OLED_WIDTH * OLED_HEIGHT / 8)


// I2C OLED address
#define OLED_I2C_ADDR     0x3C

// Font declaration (extern, define in oled.c)
extern const uint8_t Font5x7_data[96][5];
extern volatile uint8_t oled_update_flag;


// OLED buffer
extern uint8_t OLED_Buffer[OLED_BUFFER_SIZE];

//------------------ low-level ------------------
void OLED_Send_Command(uint8_t cmd);
void OLED_Send_Data(uint8_t* data, uint16_t len);

//------------------ basic ------------------
void OLED_Init(uint16_t rf_rate);
void OLED_Clear(void);
void OLED_Update(void);
void OLED_Update_Partial(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1);

//------------------ pixel ------------------
void OLED_Draw_Pixel(uint8_t x, uint8_t y, uint8_t color);

//------------------ text ------------------
void OLED_Draw_Char(uint8_t x, uint8_t y, char c, uint8_t color);
void OLED_Draw_String(uint8_t x, uint8_t y, const char* str, uint8_t color);
void OLED_Draw_Char_Big(uint8_t x, uint8_t y, char c, uint8_t size, uint8_t color);
void OLED_Draw_String_Big(uint8_t x, uint8_t y, const char* str, uint8_t size, uint8_t color);

//------------------ lines ------------------
void OLED_Draw_HLine(uint8_t x, uint8_t y, uint8_t w, uint8_t color);
void OLED_Draw_VLine(uint8_t x, uint8_t y, uint8_t h, uint8_t color);

//------------------ rectangles ------------------
void OLED_Draw_Rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
void OLED_Fill_Rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);

//------------------ circles ------------------
void OLED_Draw_Circle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color);
void OLED_Fill_Circle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color);

//------------------ numbers ------------------
void OLED_Draw_Number(uint8_t x, uint8_t y, int32_t num, uint8_t color);
void OLED_Number_Big(uint8_t x, uint8_t y, int32_t value, uint8_t size, uint8_t color);

//------------------ integer formatting ------------------
void OLED_Draw_Int(uint8_t x, uint8_t y, int32_t value, uint8_t digits);
void OLED_Draw_Int_Big(uint8_t x, uint8_t y, int32_t value, uint8_t digits, uint8_t size);

//------------------ simple delay ------------------
void delay_ms(uint32_t ms);

//-------------------Timer For Refressing-----------
void Oled_Timer_Init(uint16_t rf_rate);
uint8_t Oled_Update_Flag_Read(void);
void Oled_Update_Flag_Set(void);
void Oled_Update_Flag_Clear(void);


#endif // OLED_H