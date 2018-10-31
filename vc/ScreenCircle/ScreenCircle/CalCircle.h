#pragma once

#define MAX_COUNT 16				//最大数量
#define CIRCLE_COUNT MAX_COUNT		//实际数量

extern int border_width;			//窗口宽度
extern int border_height;			//窗口高度

extern double circle_radius;		//圆的半径
extern double base_speed;			//基准速度

extern double pos_x[MAX_COUNT];		//每个圆的位置X
extern double pos_y[MAX_COUNT];		//每个圆的位置Y
extern double spd_x[MAX_COUNT];		//每个圆的速度X
extern double spd_y[MAX_COUNT];		//每个圆的速度Y

//计算经过一个时间周期后每个圆的位置和速度
void CalCircleCycle();
