#include "robot_eye.h"
#include "oled.h"
#include "timebase.h"

#define LEFT_X 20
#define RIGHT_X 72
#define Y0 18

#define EYE_W 36
#define EYE_H 20
#define EYE_R 6       // corner radius
#define BLINK_STEPS 4

static uint8_t blink_step = 0;
static uint8_t blink_dir = 0;
static int8_t eye_offset = 0; // horizontal shift for look left/right

// -----------------------------
// Draw rounded rectangle in OLED buffer
// -----------------------------
static void OLED_Draw_RoundedRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color)
{
    for(int i=0;i<h;i++){
        for(int j=0;j<w;j++){
            int dx = (j<r)? r-j-1 : (j>=w-r)? j-(w-r) : 0;
            int dy = (i<r)? r-i-1 : (i>=h-r)? i-(h-r) : 0;
            if(dx*dx + dy*dy <= r*r)
                OLED_Draw_Pixel(x+j, y+i, color);
            else if(i>=r && i<h-r)
                OLED_Draw_Pixel(x+j, y+i, color);
            else if(j>=r && j<w-r)
                OLED_Draw_Pixel(x+j, y+i, color);
        }
    }
}

// -----------------------------
// Clear eyes in buffer
// -----------------------------
static void ClearEyes(void)
{
    OLED_Fill_Rect(LEFT_X-2,Y0-2,EYE_W+4,EYE_H+4,0);
    OLED_Fill_Rect(RIGHT_X-2,Y0-2,EYE_W+4,EYE_H+4,0);
}

// -----------------------------
// Draw both eyes with given height
// -----------------------------
static void DrawEyes(uint8_t h)
{
    OLED_Draw_RoundedRect(LEFT_X+eye_offset, Y0+(EYE_H-h)/2, EYE_W, h, EYE_R, 1);
    OLED_Draw_RoundedRect(RIGHT_X+eye_offset, Y0+(EYE_H-h)/2, EYE_W, h, EYE_R, 1);
}

// -----------------------------
// Initialize robot eyes
// -----------------------------
void RobotEye_Init(void)
{
    blink_dir = 0;
    eye_offset = 0;
    ClearEyes();
    DrawEyes(EYE_H);
}

// -----------------------------
// Blink request
// -----------------------------
void RobotEye_Blink(void)
{
    if(blink_dir == 0)
    {
        blink_dir = 1;
        blink_step = 0;
    }
}

// -----------------------------
// Look left / right / center
// -----------------------------
void RobotEye_LookLeft(void)  { eye_offset = -6; DrawEyes(EYE_H); }
void RobotEye_LookRight(void) { eye_offset = 6; DrawEyes(EYE_H); }
void RobotEye_Center(void)    { eye_offset = 0; DrawEyes(EYE_H); }

// -----------------------------
// Emotions
// -----------------------------
void RobotEye_Happy(void)  { ClearEyes(); DrawEyes(EYE_H + 4); }
void RobotEye_Angry(void)  { ClearEyes(); DrawEyes(EYE_H - 4); }
void RobotEye_Sleep(void)  { ClearEyes(); DrawEyes(4); }

// -----------------------------
// Main task (non-blocking)
// Call this inside main loop
// -----------------------------
void RobotEye_Task(void)
{
    if(blink_dir == 0) return;

    ClearEyes();

    uint8_t h = EYE_H - blink_step*4;
    DrawEyes(h);

    if(Timebase_DownCounter_SS_Continuous_Expired_Event(3))
    {
        blink_step++;
        if(blink_step >= BLINK_STEPS)
        {
            blink_step = 0;
            blink_dir = 0;
            DrawEyes(EYE_H); // reopen eyes
        }
    }
}