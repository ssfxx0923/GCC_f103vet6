#ifndef __SERVO_MOVE_SET_H
#define __SERVO_MOVE_SET_H

#include "PCA9685.h"

ServoMotion servo_init[10] = {
    {0, 90, 90, 1},
    {1, 90, 90, 1},
    {2, 90, 90, 1},
    {3, 90, 90, 1},
    {4, 90, 90, 1},
    {5, 90, 90, 1},
    {6, 90, 90, 1},
    {7, 90, 90, 1},
    {8, 47, 47, 1},
    {9, 47, 47, 1},
};

ServoMotion servo_1[2] = {             //
    {0, 167, 167, 1},
    {4,20, 20, 1},
};

    // crazyMe(9,46,46,20,30);//爪子张开
    // delay_ms(1000);
    // crazyMe(9,160,160,20,30); //爪子闭合



#endif
