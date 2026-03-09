#ifndef OLED_H
#define OLED_H

#include "stm32f401xc.h"
#include "i2c1.h"
#include "font5x7_data.h"
#include <stdio.h>

// OLED resolution
#define OLED_WIDTH      128
#define OLED_HEIGHT     64
#define OLED_BUFFER_SIZE  (OLED_WIDTH * OLED_HEIGHT / 8)
#define OLED_I2C_ADDR   0x3C  // change if your module is different

//------------------ low-level ------------------
void OLED_Send_Command(uint8_t cmd);
void OLED_Send_Data(uint8_t* data, uint16_t len);

//------------------ basic ------------------
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Update(void);
void OLED_Update_Partial(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1);

//------------------ pixel ------------------
void OLED_Draw_Pixel(uint8_t x, uint8_t y, uint8_t color);

//------------------ text ------------------
void OLED_Draw_Char(uint8_t x, uint8_t y, char c);
void OLED_Draw_String(uint8_t x, uint8_t y, const char* str);
void OLED_Draw_Number(uint8_t x, uint8_t y, int32_t num);
void OLED_Draw_Int(uint8_t x, uint8_t y, int32_t value, uint8_t digits);

//------------------ lines ------------------
void OLED_Draw_HLine(uint8_t x, uint8_t y, uint8_t w);
void OLED_Draw_VLine(uint8_t x, uint8_t y, uint8_t h);

//------------------ rectangles ------------------
void OLED_Draw_Rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void OLED_Fill_Rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

//------------------ circles ------------------
void OLED_Draw_Circle(uint8_t x0, uint8_t y0, uint8_t r);
void OLED_Fill_Circle(uint8_t x0, uint8_t y0, uint8_t r);
//------------------ utility ------------------
void delay_ms(uint32_t ms);

#endif