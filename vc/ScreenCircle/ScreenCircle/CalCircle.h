#pragma once

#define MAX_COUNT 16				//�������
#define CIRCLE_COUNT MAX_COUNT		//ʵ������

extern int border_width;			//���ڿ��
extern int border_height;			//���ڸ߶�

extern double circle_radius;		//Բ�İ뾶
extern double base_speed;			//��׼�ٶ�

extern double pos_x[MAX_COUNT];		//ÿ��Բ��λ��X
extern double pos_y[MAX_COUNT];		//ÿ��Բ��λ��Y
extern double spd_x[MAX_COUNT];		//ÿ��Բ���ٶ�X
extern double spd_y[MAX_COUNT];		//ÿ��Բ���ٶ�Y

//���㾭��һ��ʱ�����ں�ÿ��Բ��λ�ú��ٶ�
void CalCircleCycle();
