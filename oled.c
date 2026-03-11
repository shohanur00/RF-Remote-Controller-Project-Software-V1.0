#include "oled.h"

// OLED buffer
uint8_t OLED_Buffer[OLED_BUFFER_SIZE];
volatile uint8_t oled_update_flag = 0;


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
void OLED_Init(uint16_t rf_rate)
{
    I2C1_Init();
		Oled_Timer_Init(rf_rate);
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
    if(x >= OLED_WIDTH || y >= OLED_HEIGHT) return;

    if(color)
        OLED_Buffer[x + (y/8)*OLED_WIDTH] |= (1 << (y%8));
    else
        OLED_Buffer[x + (y/8)*OLED_WIDTH] &= ~(1 << (y%8));
}

//------------------ text ------------------
void OLED_Draw_Char(uint8_t x, uint8_t y, char c, uint8_t color)
{
    if(c < 32 || c > 127) c='?';

    for(uint8_t i=0;i<5;i++)
    {
        uint8_t line = Font5x7_data[c-32][i];

        for(uint8_t j=0;j<7;j++)
        {
            OLED_Draw_Pixel(x+i, y+j, (line & (1<<j)) ? color : 0);
        }
    }

    // space column
    for(uint8_t j=0;j<7;j++)
        OLED_Draw_Pixel(x+5, y+j, 0);
}


void OLED_Draw_String(uint8_t x, uint8_t y, const char* str, uint8_t color)
{
    while(*str)
    {
        OLED_Draw_Char(x, y, *str++, color);
        x += 6;
        if(x + 5 >= OLED_WIDTH) { x=0; y+=8; if(y>=OLED_HEIGHT) return; }
    }
}


void OLED_Draw_Char_Big(uint8_t x, uint8_t y, char c, uint8_t size, uint8_t color)
{
    if(c < 32 || c > 127) c='?';

    for(uint8_t i=0;i<5;i++)
    {
        uint8_t line = Font5x7_data[c-32][i];
        for(uint8_t j=0;j<7;j++)
        {
            if(line & (1<<j))
            {
                for(uint8_t xs=0; xs<size; xs++)
                    for(uint8_t ys=0; ys<size; ys++)
                        OLED_Draw_Pixel(x + i*size + xs, y + j*size + ys, color);
            }
        }
    }
}

void OLED_Draw_String_Big(uint8_t x, uint8_t y, const char *str, uint8_t size,uint8_t color)
{
    while(*str)
    {
        OLED_Draw_Char_Big(x, y, *str++, size,color);
        x += (6 * size);
    }
}

//------------------ lines ------------------
void OLED_Draw_HLine(uint8_t x, uint8_t y, uint8_t w, uint8_t color)
{
    for(uint8_t i=0;i<w;i++) OLED_Draw_Pixel(x+i,y,color);
}

void OLED_Draw_VLine(uint8_t x, uint8_t y, uint8_t h, uint8_t color)
{
    for(uint8_t i=0;i<h;i++) OLED_Draw_Pixel(x,y+i,color);
}

void OLED_Draw_Rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{
    OLED_Draw_HLine(x,y,w,color);
    OLED_Draw_HLine(x,y+h-1,w,color);
    OLED_Draw_VLine(x,y,h,color);
    OLED_Draw_VLine(x+w-1,y,h,color);
}

void OLED_Fill_Rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{
    for(uint8_t i=0;i<w;i++)
        for(uint8_t j=0;j<h;j++)
            OLED_Draw_Pixel(x+i,y+j,color);
}

//------------------ circles ------------------
void OLED_Draw_Circle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    OLED_Draw_Pixel(x0, y0+r,color);
    OLED_Draw_Pixel(x0, y0-r,color);
    OLED_Draw_Pixel(x0+r, y0,color);
    OLED_Draw_Pixel(x0-r, y0,color);

    while(x<y)
    {
        if(f>=0) { y--; ddF_y+=2; f+=ddF_y; }
        x++; ddF_x+=2; f+=ddF_x;

        OLED_Draw_Pixel(x0+x, y0+y,color);
        OLED_Draw_Pixel(x0-x, y0+y,color);
        OLED_Draw_Pixel(x0+x, y0-y,color);
        OLED_Draw_Pixel(x0-x, y0-y,color);
        OLED_Draw_Pixel(x0+y, y0+x,color);
        OLED_Draw_Pixel(x0-y, y0+x,color);
        OLED_Draw_Pixel(x0+y, y0-x,color);
        OLED_Draw_Pixel(x0-y, y0-x,color);
    }
}

void OLED_Fill_Circle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color)
{
    int16_t x=0,y=r;
    int16_t d=1-r;

    while(y>=x)
    {
        OLED_Draw_HLine(x0-x, y0-y, 2*x+1, color);
        OLED_Draw_HLine(x0-x, y0+y, 2*x+1, color);
        OLED_Draw_HLine(x0-y, y0-x, 2*y+1, color);
        OLED_Draw_HLine(x0-y, y0+x, 2*y+1, color);

        x++;
        if(d<0) d+=2*x+1;
        else { y--; d+=2*(x-y)+1; }
    }
}
//------------------ numbers ------------------
void OLED_Draw_Number(uint8_t x, uint8_t y, int32_t num,uint8_t color)
{
    char buf[12];
    sprintf(buf,"%ld",num);
    OLED_Draw_String(x,y,buf,color);
}

void OLED_Number_Big(uint8_t x, uint8_t y, int32_t value, uint8_t size,uint8_t color)
{
    char buf[12];
    sprintf(buf,"%ld",value);
    OLED_Draw_String_Big(x, y, buf, size,color);
}



//------------------ simple delay (no HAL) ------------------
void delay_ms(uint32_t ms)
{
    for(uint32_t i=0;i<ms;i++)
        for(uint32_t j=0;j<5000;j++); // approximate, tune for MCU speed
}


//--------------------Oled Refress ----------------------------

void Oled_Timer_Init(uint16_t rf_rate)
{
    uint32_t tim_clk = RCC_Get_APB2_Clock(); 
    uint32_t ppre2 = (RCC->CFGR & RCC_CFGR_PPRE2) >> 13; 
    if(ppre2 >= 4) tim_clk *= 2;

    // Timer tick frequency ~10kHz
    uint32_t tick_freq = 10000; 
    uint32_t prescaler = tim_clk / tick_freq - 1;
    if(prescaler > 0xFFFF) prescaler = 0xFFFF;

    uint32_t arr = tick_freq / rf_rate - 1;
    if(arr > 0xFFFF) arr = 0xFFFF;

    // Enable Timer9 clock
    RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;

    // Disable timer before config
    TIM9->CR1 &= ~TIM_CR1_CEN;

    TIM9->PSC = prescaler;
    TIM9->ARR = arr;
    TIM9->EGR = TIM_EGR_UG;     // update registers
    TIM9->DIER |= TIM_DIER_UIE; // enable update interrupt

    NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn); // enable Timer9 IRQ

    TIM9->CR1 |= TIM_CR1_CEN;   // start timer
}

void TIM1_BRK_TIM9_IRQHandler(void)
{
    if(TIM9->SR & TIM_SR_UIF)
    {
        TIM9->SR &= ~TIM_SR_UIF; // clear update flag
        Oled_Update_Flag_Set();
    }
}


// Read flag
uint8_t Oled_Update_Flag_Read(void)
{
    return oled_update_flag;
}

// Set flag
void Oled_Update_Flag_Set(void)
{
    oled_update_flag = 1;
}

// Clear flag
void Oled_Update_Flag_Clear(void)
{
    oled_update_flag = 0;
}