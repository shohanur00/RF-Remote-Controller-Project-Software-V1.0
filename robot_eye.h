#ifndef ROBOT_EYE_H
#define ROBOT_EYE_H

#include <stdint.h>

void RobotEye_Init(void);
void RobotEye_Task(void);
void RobotEye_Blink(void);

void RobotEye_LookLeft(void);
void RobotEye_LookRight(void);
void RobotEye_Center(void);

void RobotEye_Happy(void);
void RobotEye_Angry(void);
void RobotEye_Sleep(void);

#endif