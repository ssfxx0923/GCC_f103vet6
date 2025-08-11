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
    {0, 145, 145, 1},
    {1,35, 35, 1},
};

ServoMotion servo_2[2] = {             //
    {2, 23, 23, 1},
    {3,160, 160, 1},
};

ServoMotion servo_3[2] = {             //
    {0, 140, 140, 1},
    {1,45, 45, 1},
};

ServoMotion servo_4[2] = {             //
    {2, 23, 23, 1},
    {3,160, 160, 1},
};

ServoMotion servo_5[2] = {             //
    {0, 20, 20, 1},
    {1,160, 160, 1},
};

    // crazyMe(9,46,46,20,30);//爪子张开
    // delay_ms(1000);
    // crazyMe(9,160,160,20,30); //爪子闭合



#endif
