#ifndef __MOVE_SET_H
#define __MOVE_SET_H

#include "pid_control.h"
#include "openmv_control.h"

//OpenMV_Go_Control(50,2,1.6);          走到中心点

// 从外环返回中心
// Position_Control_Start_All(-2,30);  
// OpenMV_Turn_Control(1,2,20);
//OpenMV_Go_Control(40,1,1.2);

//从中心去到颜色识别点
//OpenMV_Turn_Control(1,2,20);
//Position_Control_Start_All(0.5,40);

//从颜色识别点回到中心
//Position_Control_Start_All(0.5,40);

//从颜色识别点去到内环
//Position_Control_Start_All(0.5,40)

//从内环去到外环
//OpenMV_Go_Control(40,1,1.2);

//从中心去到物料区
//OpenMV_Turn_Control(1,2,20);
//OpenMV_Go_Control(40,1,1.2);

//从物料区返回中心
// Position_Control_Start_All(-2,30);  
// OpenMV_Turn_Control(1,2,20);
//OpenMV_Go_Control(40,1,1.2);

//从中心去到内环
//OpenMV_Go_Control(40,1,1.2);                                                                              `

//从内环回到中心
//Position_Control_Start_All(0.5,40);



#endif