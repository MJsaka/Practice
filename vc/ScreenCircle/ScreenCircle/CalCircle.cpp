#include "stdafx.h"
#include "CalCircle.h"


#define _USE_MATH_DEFINES
#include "math.h"

int border_width = 0;			//窗口宽度
int border_height = 0;			//窗口高度

double circle_radius = 32;		//圆的半径
double base_speed = 5;			//基准速度

double pos_x[MAX_COUNT];		//每个圆的位置X
double pos_y[MAX_COUNT];		//每个圆的位置Y
double spd_x[MAX_COUNT];		//每个圆的速度X
double spd_y[MAX_COUNT];		//每个圆的速度Y


//计算圆和边界碰撞的时间点
double CalCircleBorderCollideTime(int i)
{
	double x1 = pos_x[i];
	double y1 = pos_y[i];

	double vx1 = spd_x[i];
	double vy1 = spd_y[i];

	double xmin = circle_radius;
	double xmax = border_width - circle_radius;
	double ymin = circle_radius;
	double ymax = border_height - circle_radius;

	double t;
	double tx, ty;
	if (vx1 > 0)
	{
		tx = (xmax - x1) / vx1;
	}
	else if (vx1 < 0)
	{
		tx = (xmin - x1) / vx1;
	}
	else
	{
		tx = HUGE_VAL;
	}

	if (vy1 > 0)
	{
		ty = (ymax - y1) / vy1;
	}
	else if (vy1 < 0)
	{
		ty = (ymin - y1) / vy1;
	}
	else
	{
		ty = HUGE_VAL;
	}

	t = min(tx, ty);

	return t;
}
//计算两个圆碰撞的时间点
double CalCircleCircleCollideTime(int i, int j)
{
	double x_i = pos_x[i];
	double y_i = pos_y[i];
	double x_j = pos_x[j];
	double y_j = pos_y[j];

	double vx_i = spd_x[i];
	double vy_i = spd_y[i];
	double vx_j = spd_x[j];
	double vy_j = spd_y[j];

	double dx = x_i - x_j;
	double dy = y_i - y_j;
	double dvx = vx_i - vx_j;
	double dvy = vy_i - vy_j;

	double A = dvx*dvx + dvy*dvy;
	double B = 2 * (dx*dvx + dy*dvy);
	double C = (dx*dx + dy*dy) - 4 * circle_radius * circle_radius;

	double t = (-B - sqrt(B*B - 4 * A*C)) / (2 * A);
	return t;
}
//计算两个圆碰撞后的速度
void CalCircleSpeedAfterCircleCircleCollide(int i, int j)
{
	double x_i_before = pos_x[i];
	double y_i_before = pos_y[i];
	double x_j_before = pos_x[j];
	double y_j_before = pos_y[j];

	double dx = x_i_before - x_j_before;
	double dy = y_i_before - y_j_before;
	double dis = sqrt(dx*dx + dy*dy);

	double sina = dy / dis;
	double cosa = dx / dis;

	double vx_i_before = spd_x[i];
	double vy_i_before = spd_y[i];
	double vx_j_before = spd_x[j];
	double vy_j_before = spd_y[j];

	double vn_i_before = vy_i_before * sina + vx_i_before * cosa; //圆心连线方向
	double vt_i_before = vy_i_before * cosa - vx_i_before * sina; //切线方向
	double vn_j_before = vy_j_before * sina + vx_j_before * cosa;
	double vt_j_before = vy_j_before * cosa - vx_j_before * sina;

	double vn_i_after = vn_j_before; //圆心连线方向速度交换
	double vt_i_after = vt_i_before; //切线方向速度不变
	double vn_j_after = vn_i_before;
	double vt_j_after = vt_j_before;

	double vx_i_after = vn_i_after * cosa - vt_i_after * sina;
	double vy_i_after = vn_i_after * sina + vt_i_after * cosa;
	double vx_j_after = vn_j_after * cosa - vt_j_after * sina;
	double vy_j_after = vn_j_after * sina + vt_j_after * cosa;

	spd_x[i] = vx_i_after;
	spd_y[i] = vy_i_after;
	spd_x[j] = vx_j_after;
	spd_y[j] = vy_j_after;
}

//计算圆和边界碰撞后的速度
void CalCircleSpeedAfterCircleBorderCollide(int i)
{
	double dx = min(fabsl(pos_x[i]), fabsl(pos_x[i] - border_width));
	double dy = min(fabsl(pos_y[i]), fabsl(pos_y[i] - border_height));

	if (dx < dy)
	{
		spd_x[i] *= -1;
	}
	else
	{
		spd_y[i] *= -1;
	}
}
//计算每个圆经过一段时间后的位置
void CalCirclePosAfterTime(int i, double t)
{
	pos_x[i] += spd_x[i] * t;
	pos_y[i] += spd_y[i] * t;
}
//计算经过一个时间周期后每个圆的位置和速度
//1 找到本时间周期剩于时间内最早发生碰撞的时间点
//2 计算发生碰撞时所有圆的位置
//3 计算发生碰撞后所有圆的速度
//4 重复此过程直至本周期时间结束
void CalCircleCycle()
{
	double RestTime = 1.0;
	do
	{
		bool hasCollide = false;//本周期剩于时间内是否会发生碰撞
		double minCollideTime = RestTime;//最早发生碰撞的时间点

		double CircleCircleCollideTime[CIRCLE_COUNT][CIRCLE_COUNT];//两个圆碰撞时间点
		double CircleBorderCollideTime[CIRCLE_COUNT];//圆与边界碰撞时间点

		for (int i = 0; i < CIRCLE_COUNT; i++)
		{//检测圆与边界在本周期剩于时间内是否有碰撞
			double t = CalCircleBorderCollideTime(i);

			if (t >= 0 && t <= RestTime)
			{
				hasCollide = true;
				CircleBorderCollideTime[i] = t;
				if (t < minCollideTime)
				{
					minCollideTime = t;
				}
			}
			else
			{
				CircleBorderCollideTime[i] = RestTime + 0.1;
			}
		}

		for (int i = 0; i < CIRCLE_COUNT; i++)
		{
			for (int j = i; j < CIRCLE_COUNT; j++)
			{//检测两圆在本周期剩于时间内是否有碰撞
				double t = CalCircleCircleCollideTime(i, j);

				if (t >= 0 && t <= RestTime)
				{
					hasCollide = true;
					CircleCircleCollideTime[i][j] = t;
					if (t < minCollideTime)
					{
						minCollideTime = t;
					}
				}
				else
				{
					CircleCircleCollideTime[i][j] = RestTime + 0.01;
				}
			}
		}

		for (int i = 0; i < CIRCLE_COUNT; i++)
		{//对每个圆计算新的位置
			CalCirclePosAfterTime(i, minCollideTime);
		}

		if (hasCollide)
		{//有碰撞发生，计算新的速度
			for (int i = 0; i < CIRCLE_COUNT; i++)
			{
				for (int j = i; j < CIRCLE_COUNT; j++)
				{//对每对碰撞的圆计算碰撞后的速度
					if (CircleCircleCollideTime[i][j] == minCollideTime)
					{
						CalCircleSpeedAfterCircleCircleCollide(i, j);
					}
				}
			}
			for (int i = 0; i < CIRCLE_COUNT; i++)
			{//对每个与边界碰撞的圆计算新的速度
				if (CircleBorderCollideTime[i] == minCollideTime)
				{
					CalCircleSpeedAfterCircleBorderCollide(i);
				}
			}
		}
		//更新剩于时间
		RestTime -= minCollideTime;
	} while (RestTime > 0.01);
}

