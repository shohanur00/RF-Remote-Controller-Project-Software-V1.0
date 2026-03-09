#include "oled.h"

// OLED buffer
uint8_t OLED_Buffer[OLED_BUFFER_SIZE];

//------------------ low-level ------------------
void OLED_Send_Command(uint8_t cmd)
{
    if(I2C1_Start()) return;
    if(I2C1_Send_Addr(OLED_I2C_ADDR, 0)) return;

    I2C1_Write_Data(0x00); // control byte = command
    I2C1_Write_Data(cmd);
    I2C1_Stop();
}

void OLED_Send_Data(uint8_t* data, uint16_t len)
{
    if(I2C1_Start()) return;
    if(I2C1_Send_Addr(OLED_I2C_ADDR, 0)) return;

    I2C1_Write_Data(0x40); // control byte = data
    for(uint16_t i=0; i<len; i++)
        I2C1_Write_Data(data[i]);

    I2C1_Stop();
}

//------------------ basic ------------------
void OLED_Init(void)
{
    I2C1_Init();
    OLED_Clear();

    OLED_Send_Command(0xAE); // display off
    OLED_Send_Command(0xD5); OLED_Send_Command(0x80);
    OLED_Send_Command(0xA8); OLED_Send_Command(0x3F);
    OLED_Send_Command(0xD3); OLED_Send_Command(0x00);
    OLED_Send_Command(0x40);
    OLED_Send_Command(0x8D); OLED_Send_Command(0x14);
    OLED_Send_Command(0x20); OLED_Send_Command(0x00);
    OLED_Send_Command(0xA1); OLED_Send_Command(0xC8);
    OLED_Send_Command(0xDA); OLED_Send_Command(0x12);
    OLED_Send_Command(0x81); OLED_Send_Command(0xCF);
    OLED_Send_Command(0xD9); OLED_Send_Command(0xF1);
    OLED_Send_Command(0xDB); OLED_Send_Command(0x40);
    OLED_Send_Command(0xA4); OLED_Send_Command(0xA6);
    OLED_Send_Command(0xAF); // display on
}

void OLED_Clear(void)
{
    for(int i=0;i<OLED_BUFFER_SIZE;i++)
        OLED_Buffer[i]=0;
}

void OLED_Update(void)
{
    for(uint8_t page=0; page<8; page++)
    {
        OLED_Send_Command(0xB0 + page);
        OLED_Send_Command(0x00);
        OLED_Send_Command(0x10);
        OLED_Send_Data(&OLED_Buffer[page*OLED_WIDTH], OLED_WIDTH);
    }
}

// Partial update: update a rectangle (x0,y0) to (x1,y1)
void OLED_Update_Partial(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1)
{
    if(x1>=OLED_WIDTH) x1=OLED_WIDTH-1;
    if(y1>=OLED_HEIGHT) y1=OLED_HEIGHT-1;

    uint8_t page_start = y0/8;
    uint8_t page_end   = y1/8;

    for(uint8_t page=page_start; page<=page_end; page++)
    {
        OLED_Send_Command(0xB0 + page);
        OLED_Send_Command(0x00 + (x0 & 0x0F));
        OLED_Send_Command(0x10 + (x0 >> 4));

        uint16_t len = x1 - x0 + 1;
        OLED_Send_Data(&OLED_Buffer[page*OLED_WIDTH + x0], len);
    }
}

//------------------ pixel ------------------
void OLED_Draw_Pixel(uint8_t x, uint8_t y, uint8_t color)
{
    if(x>=OLED_WIDTH || y>=OLED_HEIGHT) return;

    if(color)
        OLED_Buffer[x + (y/8)*OLED_WIDTH] |=  (1<<(y%8));
    else
        OLED_Buffer[x + (y/8)*OLED_WIDTH] &= ~(1<<(y%8));
}

//------------------ text ------------------
void OLED_Draw_Char(uint8_t x, uint8_t y, char c)
{
    if(c < 32 || c > 127) c = '?';

    OLED_Send_Command(0xB0 + y);            // page
    OLED_Send_Command(0x00 + (x & 0x0F));   // low nibble
    OLED_Send_Command(0x10 + (x >> 4));     // high nibble

    OLED_Send_Data((uint8_t*)Font5x7_data[c-32], 5);

    uint8_t space = 0;
    OLED_Send_Data(&space, 1);
}

void OLED_Draw_String(uint8_t x, uint8_t y, const char* str)
{
    while(*str)
    {
        OLED_Draw_Char(x, y, *str++);
        x+=6;
        if(x+5>=OLED_WIDTH) { x=0; y+=8; }
    }
}

//------------------ lines ------------------
void OLED_Draw_HLine(uint8_t x, uint8_t y, uint8_t w)
{
    for(uint8_t i=0;i<w;i++) OLED_Draw_Pixel(x+i,y,1);
}

void OLED_Draw_VLine(uint8_t x, uint8_t y, uint8_t h)
{
    for(uint8_t i=0;i<h;i++) OLED_Draw_Pixel(x,y+i,1);
}

//------------------ rectangles ------------------
void OLED_Draw_Rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    OLED_Draw_HLine(x,y,w);
    OLED_Draw_HLine(x,y+h-1,w);
    OLED_Draw_VLine(x,y,h);
    OLED_Draw_VLine(x+w-1,y,h);
}

void OLED_Fill_Rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    for(uint8_t i=0;i<w;i++)
        for(uint8_t j=0;j<h;j++)
            OLED_Draw_Pixel(x+i,y+j,1);
}

//------------------ circles ------------------
void OLED_Draw_Circle(uint8_t x0, uint8_t y0, uint8_t r)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    OLED_Draw_Pixel(x0, y0+r, 1);
    OLED_Draw_Pixel(x0, y0-r, 1);
    OLED_Draw_Pixel(x0+r, y0, 1);
    OLED_Draw_Pixel(x0-r, y0, 1);

    while(x<y)
    {
        if(f>=0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        OLED_Draw_Pixel(x0+x, y0+y,1);
        OLED_Draw_Pixel(x0-x, y0+y,1);
        OLED_Draw_Pixel(x0+x, y0-y,1);
        OLED_Draw_Pixel(x0-x, y0-y,1);
        OLED_Draw_Pixel(x0+y, y0+x,1);
        OLED_Draw_Pixel(x0-y, y0+x,1);
        OLED_Draw_Pixel(x0+y, y0-x,1);
        OLED_Draw_Pixel(x0-y, y0-x,1);
    }
}

// Filled circle using Midpoint circle algorithm
void OLED_Fill_Circle(uint8_t x0, uint8_t y0, uint8_t r)
{
    int16_t x = 0;
    int16_t y = r;
    int16_t d = 1 - r;

    while (y >= x)
    {
        // Draw horizontal lines between symmetric points
        OLED_Draw_HLine(x0 - x, y0 - y, 2*x + 1);
        OLED_Draw_HLine(x0 - x, y0 + y, 2*x + 1);
        OLED_Draw_HLine(x0 - y, y0 - x, 2*y + 1);
        OLED_Draw_HLine(x0 - y, y0 + x, 2*y + 1);

        x++;
        if (d < 0)
        {
            d += 2*x + 1;
        }
        else
        {
            y--;
            d += 2*(x - y) + 1;
        }
    }
}

//------------------ numbers ------------------
void OLED_Draw_Number(uint8_t x, uint8_t y, int32_t num)
{
    char buf[12];
    sprintf(buf,"%ld",num);
    OLED_Draw_String(x,y,buf);
}

void OLED_Draw_Int(uint8_t x, uint8_t y, int32_t value, uint8_t digits)
{
    char buf[12];

    switch(digits)
    {
        case 1:  sprintf(buf,"%1ld",value); break;
        case 2:  sprintf(buf,"%02ld",value); break;
        case 3:  sprintf(buf,"%03ld",value); break;
        case 4:  sprintf(buf,"%04ld",value); break;
        case 5:  sprintf(buf,"%05ld",value); break;
        case 6:  sprintf(buf,"%06ld",value); break;
        case 7:  sprintf(buf,"%07ld",value); break;
        case 8:  sprintf(buf,"%08ld",value); break;
        case 9:  sprintf(buf,"%09ld",value); break;
        case 10: sprintf(buf,"%010ld",value); break;

        default: sprintf(buf,"%ld",value); break;
    }

    OLED_Draw_String(x,y,buf);
}

//------------------ simple delay (no HAL) ------------------
void delay_ms(uint32_t ms)
{
    for(uint32_t i=0;i<ms;i++)
        for(uint32_t j=0;j<5000;j++); // approximate, tune for MCU speed
}