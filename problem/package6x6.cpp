#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

/********************************************
 * [问题描述]
 *   一个工厂制造的产品形状都是长方体，他们的高度都是h，底面的长和宽都相等，
 *   一共有6个型号，他们的长宽分别为： 1*1、2*2、3*3、4*4、5*5、6*6。
 *   这些产品通常使用一个6*6*h的长方体包裹包装，然后邮寄给客户。
 *   因为邮寄费用很贵，所以工厂要想方设法地减少每个订单运送时的包裹数量。
 *   他们很需要有一个好的程序解决这个问题从而节省费用。现在这个程序由你来设计。
 * [输入格式]
 *   第 1 行：包含六个整数：a,b,c,d,e,f，分别代表一个订单中1*1、2*2、3*3、4*4、5*5、6*6产品的数量。
 *
 * [输出格式]
 *   第 1 行：一个整数，表示输入的订单需要的最少包裹数量。
 *
 * [输入样例]
 *   0 0 4 0 0 1
 *
 * [输出样例]
 *   2
 *
 * [数据范围]
 *   0<=a,b,c,d,e,f<=100000000
 */

//思路:
//6x6的只能用1个箱装,装完即满
//5x5的只能用1个箱装,装完剩1x1的空间11个
//4x4的只能用1个箱装,装完剩2x2的空间5个
//
//3x3的1个箱可以装4个
//若落单的是3个,剩于3x3空间1个,可以装1个2x2加5个1x1
//若落单的是2个,剩于3x3空间2个,可以装3个2x2加6个1x1
//若落单的是1个,剩于3x3空间3个,可以装5个2x2加7个1x1
//
//装完3x3,4x4,5x5之后,将2x2先往可以填的空里填,没有可填的空之后,再单独装
//2x2的1个箱可以装9个
//
//最后把1x1的往可以填的空里填,没有空位后,再单独装
//1x1的1个箱可以装36个

int cal_count(int Count[])
{
	int Space[7] = {0};//装完产品之后,各种类型的空间的数量
	int BoxCount = Count[6] + Count[5] + Count[4] + Count[3]/4;//总的箱子数
	Space[2] = Count[4] * 5;
	Space[1] = Count[5] * 11;
	switch( Count[3] % 4)
	{
		case 3:
			BoxCount++;
			Space[2]+=1;
			Space[1]+=5;
			break;
		case 2:
			BoxCount++;
			Space[2]+=3;
			Space[1]+=6;
			break;
		case 1:
			BoxCount++;
			Space[2]+=5;
			Space[1]+=7;
			break;
		case 0:
			break;
	}
	if(Count[2] >= Space[2])
	{//将所有2x2的空间填上
		Count[2] -= Space[2];

		BoxCount += Count[2] / 9 + Count[2] % 9 == 0 ? 0 : 1;
		Space[2] = Count[2] % 9 == 0 ? 0 : 9 - Count[2] % 9;
	}else
	{//所有2x2产品都可以找到空填上
		Space[2] -= Count[2];
		Count[2] = 0;
	}
	Space[1] += Space[2] * 4;//将Space[2]转为Space[1]用
	if (Count[1] > Space[1])
	{
		Count[1] -= Space[1];

		BoxCount += Count[1] / 36 + Count[1] % 36 == 0 ? 0 : 1;
		Space[1] = Count[1] % 36 == 0 ? 0 : 36 - Count[1] % 36;
	}else
	{
		Space[1] -= Count[1];
		Count[1] = 0;
	}

	return BoxCount;
}

void test1()
{
	int Count[7] = {0};//每种型号的数量
	for (int i=1; i<=6; i++)
	{
		cout << "Input " << i << "x" << i << " Count" << endl;
		int num;
		cin >> num;
		while (num<0 || num > 100000000)
		{
			cout << "Input error, Count should in [0,100000000), please input again" << endl;
			cin >> num;
		}
		Count[i] = num;
	}

	int BoxCount = cal_count(Count);
	cout << "Need BoxCount:" << BoxCount << endl;
}

void test2()
{
	int Count[7] = {0};//每种型号的数量

	string filepath1 = "D:\\1";
	ifstream is(filepath1);
	string line;
	while(getline(is, line))
	{
		stringstream ss(line);
		bool thiscontinue = false;
		for (int i=1; i<=6; i++)
		{
			int num;
			ss >> num;
			if (!ss || num<0 || num > 100000000)
			{
				thiscontinue = true;
				break;
			}
			Count[i] = num;
		}

		if(thiscontinue)
			continue;

		for (int i=1; i<=6; i++)
		{
			cout << Count[i] << " ";
		}
		int BoxCount = cal_count(Count);
		cout << "--> " << BoxCount << endl;
	}
}

int main()
{
	//test1();
	test2();
}
