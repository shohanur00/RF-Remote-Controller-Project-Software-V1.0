#include "animation.h"
#include "oled.h"
#include <math.h>
#include<string.h>


void ANIM_Booting(uint32_t duration_ms)
{
    uint8_t bar_width  = 100;
    uint8_t bar_height = 10;

    // Position: center of OLED
    uint8_t bar_x = (OLED_WIDTH - bar_width) / 2;
    uint8_t bar_y = (OLED_HEIGHT - bar_height) / 2;

    uint32_t frame_delay = 20; // 20ms per frame for smoothness
    uint32_t total_frames = duration_ms / frame_delay;

    for(uint32_t frame = 0; frame <= total_frames; frame++)
    {
        OLED_Clear();

        // calculate current progress
        uint8_t progress = (frame * 100) / total_frames;

        // draw progress bar using professional function
        ANIM_ProgressBar_Pro(bar_x, bar_y, bar_width, bar_height, progress, 100, OLED_WHITE);

        OLED_Update();

        // simple delay loop (~frame_delay)
        for(volatile uint32_t d = 0; d < 40000; d++); // adjust for MCU speed
    }

    // Optional: hold final screen for 500ms
    for(volatile uint32_t d = 0; d < 1000000; d++);
}

void ANIM_ProgressBar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t progress, uint8_t max, uint8_t color)
{
    if(progress > max) progress = max;

    // Calculate fill width proportional to progress
    uint8_t fill = (width - 2) * progress / max;

    // Draw outer rectangle (border)
    for(uint8_t i = 0; i < width; i++)
    {
        OLED_Draw_Pixel(x + i, y, color);
        OLED_Draw_Pixel(x + i, y + height - 1, color);
    }
    for(uint8_t i = 0; i < height; i++)
    {
        OLED_Draw_Pixel(x, y + i, color);
        OLED_Draw_Pixel(x + width - 1, y + i, color);
    }

    // Fill the inner bar
    for(uint8_t i = 0; i < fill; i++)
    {
        for(uint8_t j = 1; j < height - 1; j++)
        {
            OLED_Draw_Pixel(x + 1 + i, y + j, color);
        }
    }
}


void ANIM_ProgressBar_Pro(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t progress, uint8_t max, uint8_t color)
{
    if(progress > max) progress = max;

    uint8_t fill = (width - 2) * progress / max;

    // -------- Draw border --------
    for(uint8_t i = 0; i < width; i++)
    {
        OLED_Draw_Pixel(x + i, y, color);               // top
        OLED_Draw_Pixel(x + i, y + height - 1, color);  // bottom
    }
    for(uint8_t i = 0; i < height; i++)
    {
        OLED_Draw_Pixel(x, y + i, color);               // left
        OLED_Draw_Pixel(x + width - 1, y + i, color);   // right
    }

    // -------- Rounded corners (1 px) --------
    OLED_Draw_Pixel(x + 1, y + 1, color);                 // top-left inner
    OLED_Draw_Pixel(x + width - 2, y + 1, color);        // top-right inner
    OLED_Draw_Pixel(x + 1, y + height - 2, color);       // bottom-left inner
    OLED_Draw_Pixel(x + width - 2, y + height - 2, color);// bottom-right inner

    // -------- Fill bar --------
    for(uint8_t j = 1; j < height - 1; j++)
    {
        for(uint8_t i = 0; i < fill; i++)
        {
            OLED_Draw_Pixel(x + 1 + i, y + j, color);
        }
    }

    // -------- Percentage text --------
    char buf[5];
    uint8_t percent = (progress * 100) / max;
    sprintf(buf, "%d%%", percent);

    uint8_t text_x = x + (width - 6 * strlen(buf)) / 2;
    uint8_t text_y = y + (height - 8) / 2; // assuming 5x7 font
    OLED_Draw_String(text_x, text_y, buf, color);
}


void ANIM_UploadingDots(uint8_t x,uint8_t y,uint8_t step,uint8_t color)
{

    for(uint8_t i=0;i<(step%4);i++)
    {
        OLED_Draw_Char(x+(i*6),y,'.',color);
    }

}


void ANIM_Spinner(uint8_t x,uint8_t y,uint8_t step,uint8_t color)
{

    char spin[4]={'|','/','-','\\'};

    OLED_Draw_Char(x,y,spin[step%4],color);

}


void ANIM_BouncingBall(uint8_t *x,uint8_t *y,int8_t *dx,int8_t *dy,uint8_t r,uint8_t color)
{

    for(int i=-r;i<=r;i++)
    {
        for(int j=-r;j<=r;j++)
        {
            if(i*i+j*j<=r*r)
            {
                OLED_Draw_Pixel(*x+i,*y+j,color);
            }
        }
    }

    *x+=*dx;
    *y+=*dy;

    if(*x<=r || *x>=127-r) *dx=-*dx;
    if(*y<=r || *y>=63-r) *dy=-*dy;

}


void ANIM_LoadingBar(uint8_t x,uint8_t y,uint8_t width,uint8_t step,uint8_t color)
{

    uint8_t pos=step%width;

    for(uint8_t i=0;i<width;i++)
    {
        if(i==pos)
        {
            for(uint8_t j=0;j<5;j++)
            OLED_Draw_Pixel(x+i,y+j,color);
        }
    }

}

//------------------ Pulse Circle ------------------
void ANIM_PulseCircle(uint8_t x0, uint8_t y0, uint8_t min_r, uint8_t max_r, uint8_t step, uint8_t color)
{
    static uint8_t r = 0;
    static int8_t dir = 1;
    static uint8_t initialized = 0;

    if(!initialized)
    {
        r = min_r;   // run-time initialization
        initialized = 1;
    }

    OLED_Clear();
    OLED_Fill_Circle(x0, y0, r, color);
    OLED_Update();

    r += dir * step;
    if(r >= max_r) dir = -1;
    if(r <= min_r) dir = 1;
}

//------------------ Blinking Rectangle ------------------
void ANIM_BlinkRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color, uint32_t duration_ms)
{
    static uint8_t state = 0;

    OLED_Clear();
    if(state) OLED_Fill_Rect(x, y, w, h, color);
    OLED_Update();

    state = !state;
    delay_ms(duration_ms);
}

//------------------ Scrolling Text ------------------
void ANIM_ScrollText(uint8_t y, const char *str, uint8_t speed, uint8_t color)
{
    static int16_t x = OLED_WIDTH;
    int16_t len = 0;
    const char *ptr = str;

    while(*ptr++) len += 6;  // each char width including space

    OLED_Clear();
    OLED_Draw_String(x, y, str, color);
    OLED_Update();

    x -= speed;
    if(x + len < 0) x = OLED_WIDTH;
}

//------------------ Wave Animation ------------------
void ANIM_Wave(uint8_t x0, uint8_t y0, uint8_t length, uint8_t amplitude, uint8_t step, uint8_t color)
{
    static uint8_t phase = 0;

    OLED_Clear();
    for(uint8_t x=0; x<length; x++)
    {
        uint8_t y = y0 + (uint8_t)(amplitude * sinf((float)(x + phase) * 0.2f));
        OLED_Draw_Pixel(x0 + x, y, color);
    }
    OLED_Update();

    phase += step;
}

//------------------ Rotating Circles ------------------
void ANIM_RotatingCircles(uint8_t x0, uint8_t y0, uint8_t r, uint8_t num_circles, uint8_t step, uint8_t color)
{
    static uint8_t angle = 0;

    OLED_Clear();
    for(uint8_t i=0;i<num_circles;i++)
    {
        float theta = ((float)i/num_circles)*2.0f*3.14159f + ((float)angle*3.14159f/180.0f);
        int16_t cx = x0 + (int16_t)(r * cosf(theta));
        int16_t cy = y0 + (int16_t)(r * sinf(theta));
        OLED_Fill_Circle(cx, cy, 2, color);
    }
    OLED_Update();

    angle += step;
    if(angle >= 360) angle = 0;
}