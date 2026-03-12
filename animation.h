#ifndef ANIMATION_H
#define ANIMATION_H

#include "stm32f401xc.h"
#include "oled.h"  // include your OLED functions

#define OLED_WHITE 1
#define OLED_BLACK 0
#define OLED_INVERT 2  // optional

//------------------ booting ------------------
void ANIM_Booting(uint32_t duration_ms);

//------------------ progress bar ------------------
void ANIM_ProgressBar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t progress, uint8_t max, uint8_t color);
void ANIM_ProgressBar_Pro(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t progress, uint8_t max, uint8_t color);

//------------------ uploading dots ------------------
void ANIM_UploadingDots(uint8_t x,uint8_t y,uint8_t step,uint8_t color);

//------------------ spinner ------------------
void ANIM_Spinner(uint8_t x,uint8_t y,uint8_t step,uint8_t color);

//------------------ bouncing ball ------------------
void ANIM_BouncingBall(uint8_t *x,uint8_t *y,int8_t *dx,int8_t *dy,uint8_t r,uint8_t color);

//------------------ loading bar ------------------
void ANIM_LoadingBar(uint8_t x,uint8_t y,uint8_t width,uint8_t step,uint8_t color);

//------------------ new animations ------------------

// Pulse circle animation (circle grows and shrinks)
void ANIM_PulseCircle(uint8_t x0, uint8_t y0, uint8_t min_r, uint8_t max_r, uint8_t step, uint8_t color);

// Blinking rectangle
void ANIM_BlinkRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color, uint32_t duration_ms);

// Scrolling text
void ANIM_ScrollText(uint8_t y, const char *str, uint8_t speed, uint8_t color);

// Wave animation (sine wave moving horizontally)
void ANIM_Wave(uint8_t x0, uint8_t y0, uint8_t length, uint8_t amplitude, uint8_t step, uint8_t color);

// Circle rotate animation (multiple circles rotate around a center)
void ANIM_RotatingCircles(uint8_t x0, uint8_t y0, uint8_t r, uint8_t num_circles, uint8_t step, uint8_t color);

#endif