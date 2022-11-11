#include "stdafx.h"
#include "CalCircle.h"


#define _USE_MATH_DEFINES
#include "math.h"

int border_width = 0;			//���ڿ��
int border_height = 0;			//���ڸ߶�

double circle_radius = 64;		//Բ�İ뾶
double base_speed = 1;			//��׼�ٶ�

double pos_x[MAX_COUNT];		//ÿ��Բ��λ��X
double pos_y[MAX_COUNT];		//ÿ��Բ��λ��Y
double spd_x[MAX_COUNT];		//ÿ��Բ���ٶ�X
double spd_y[MAX_COUNT];		//ÿ��Բ���ٶ�Y


//����Բ�ͱ߽���ײ��ʱ���
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

	double x2 = x1 + vx1 * t;
	double y2 = y1 + vy1 * t;

	return t;
}
//��������Բ��ײ��ʱ���
//����ײʱ���ʱ Բ�ľ������ֱ��
double CalCircleCircleCollideTime(int i, int j)
{
	double x_i = pos_x[i];
	double y_i = pos_y[i];
	double x_j = pos_x[j];
	double y_j = pos_y[j];

	double dx = x_i - x_j;
	double dy = y_i - y_j;

	double vx_i = spd_x[i];
	double vy_i = spd_y[i];
	double vx_j = spd_x[j];
	double vy_j = spd_y[j];

	double dvx = vx_i - vx_j;
	double dvy = vy_i - vy_j;

	double A = dvx*dvx + dvy*dvy;
	double B = 2 * (dx*dvx + dy*dvy);
	double C = (dx*dx + dy*dy) - 4 * circle_radius * circle_radius;

	double t = (-B - sqrt(B*B - 4 * A*C)) / (2 * A);

	double x_i2 = x_i + vx_i * t;
	double y_i2 = y_i + vy_i * t;
	double x_j2 = x_j + vx_j * t;
	double y_j2 = y_j + vy_j * t;

	double dx2 = x_i2 - x_j2;
	double dy2 = y_i2 - y_j2;

	double d1 = dx*dx + dy*dy;
	double d2 = dx2*dx2 + dy2*dy2;
	double d4 = 2 * circle_radius * 2 * circle_radius;

	if (d1 <= d4)
	{
		if (t == 0)
		{
			return t;
		}
		return t;
	}
	return t;
}
//��������Բ��ײ����ٶ�
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

	double vn_i_before = vy_i_before * sina + vx_i_before * cosa; //Բ�����߷���
	double vt_i_before = vy_i_before * cosa - vx_i_before * sina; //���߷���
	double vn_j_before = vy_j_before * sina + vx_j_before * cosa;
	double vt_j_before = vy_j_before * cosa - vx_j_before * sina;

	double vn_i_after = vn_j_before; //Բ�����߷����ٶȽ���
	double vt_i_after = vt_i_before; //���߷����ٶȲ���
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

//����Բ�ͱ߽���ײ����ٶ�
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
//����ÿ��Բ����һ��ʱ����λ��
void CalCirclePosAfterTime(int i, double t)
{
	pos_x[i] += spd_x[i] * t;
	pos_y[i] += spd_y[i] * t;
}
//���㾭��һ��ʱ�����ں�ÿ��Բ��λ�ú��ٶ�
//1 �ҵ���ʱ������ʣ��ʱ�������緢����ײ��ʱ���
//2 ���㷢����ײʱ����Բ��λ��
//3 ���㷢����ײ������Բ���ٶ�
//4 �ظ��˹���ֱ��������ʱ�����
void CalCircleCycle()
{
	double RestTime = 1.0;
	do
	{
		bool hasCollide = false;//������ʣ��ʱ�����Ƿ�ᷢ����ײ
		double minCollideTime = RestTime;//���緢����ײ��ʱ���

		double CircleCircleCollideTime[CIRCLE_COUNT][CIRCLE_COUNT];//����Բ��ײʱ���
		double CircleBorderCollideTime[CIRCLE_COUNT];//Բ��߽���ײʱ���

		bool CircleCircleHasCollide[CIRCLE_COUNT][CIRCLE_COUNT];//����Բ�Ƿ�����ײ����
		bool CircleBorderHasCollide[CIRCLE_COUNT];//Բ��߽��Ƿ�����ײ����
		bool CircleHasCollide[CIRCLE_COUNT];//Բ�Ƿ�����ײ����

		for (int i = 0; i < CIRCLE_COUNT; i++)
		{
			CircleHasCollide[i] = false;
			CircleBorderHasCollide[i] = false;
			CircleBorderCollideTime[i] = RestTime + 0.1;
			for (int j = 0; j < CIRCLE_COUNT; j++)
			{
				CircleCircleHasCollide[i][j] = false;
				CircleCircleCollideTime[i][j] = RestTime + 0.01;
			}
		}
		
		for (int i = 0; i < CIRCLE_COUNT; i++)
		{//���Բ��߽��ڱ�����ʣ��ʱ�����Ƿ�����ײ
			double t = CalCircleBorderCollideTime(i);

			if (t > 0 && t <= RestTime)
			{
				hasCollide = true;
				CircleHasCollide[i] = true;
				CircleBorderHasCollide[i] = true;
				CircleBorderCollideTime[i] = t;
				if (t < minCollideTime)
				{
					minCollideTime = t;
				}
			}
		}

		for (int i = 0; i < CIRCLE_COUNT; i++)
		{
			for (int j = i+1; j < CIRCLE_COUNT; j++)
			{//�����Բ�ڱ�����ʣ��ʱ�����Ƿ�����ײ
				double t = CalCircleCircleCollideTime(i, j);

				if (t > 0 && t <= RestTime)
				{
					hasCollide = true;
					CircleHasCollide[i] = true;
					CircleHasCollide[j] = true;
					CircleCircleHasCollide[i][j] = true;
					CircleCircleCollideTime[i][j] = t;
					if (t < minCollideTime)
					{
						minCollideTime = t;
					}
				}
			}
		}

		if (minCollideTime == 0)
		{
			break;
		}

		for (int i = 0; i < CIRCLE_COUNT; i++)
		{//��ÿ��Բ�����µ�λ��
			CalCirclePosAfterTime(i, minCollideTime);
		}

		//���ھ������� ���ܵ��¼������µ�λ��ʱ �Ѿ��ﵽ��ײλ��
		//�ټ���һ����ײʱ�� ����ײʱ���С ��Ϊ�Ѿ���ײ

		for (int i = 0; i < CIRCLE_COUNT; i++)
		{//���Բ��߽��ڱ�����ʣ��ʱ�����Ƿ�����ײ
			double t = CalCircleBorderCollideTime(i);

			if (abs(t) < 1e-15)
			{
				CircleHasCollide[i] = true;
				CircleBorderHasCollide[i] = true;
			}
		}

		for (int i = 0; i < CIRCLE_COUNT; i++)
		{
			for (int j = i + 1; j < CIRCLE_COUNT; j++)
			{//�����Բ�ڱ�����ʣ��ʱ�����Ƿ�����ײ
				double t = CalCircleCircleCollideTime(i, j);

				if (abs(t) < 1e-15)
				{
					CircleHasCollide[i] = true;
					CircleHasCollide[j] = true;
					CircleCircleHasCollide[i][j] = true;
				}
			}
		}
		
		if (hasCollide)
		{//����ײ�����������µ��ٶ�
			for (int i = 0; i < CIRCLE_COUNT; i++)
			{
				for (int j = i+1; j < CIRCLE_COUNT; j++)
				{//��ÿ����ײ��Բ������ײ����ٶ�
					if (CircleCircleHasCollide[i][j])
					{
						CalCircleSpeedAfterCircleCircleCollide(i, j);
					}
				}
			}
			for (int i = 0; i < CIRCLE_COUNT; i++)
			{//��ÿ����߽���ײ��Բ�����µ��ٶ�
				if (CircleBorderHasCollide[i])
				{
					CalCircleSpeedAfterCircleBorderCollide(i);
				}
			}
		}
		//����ʣ��ʱ��
		RestTime -= minCollideTime;
	} while (RestTime > 0);
}

