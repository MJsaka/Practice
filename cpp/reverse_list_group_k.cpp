#include<string>
#include<sstream>
#include<iostream>
#include<iomanip>
using namespace std;


#define FORMAT setw(5) << setfill('0')

struct node
{
	int data;
	int next;
	int prev;
}q[100000];

int head, tail, N, K;



// 得到首行数据
void InputHead()
{
	string line;
	getline(cin, line, '\n');
	istringstream stream(line);
	stream >> head;
	stream >> N;
	stream >> K;
}


// 获取其他行数据,并得到尾节点地址
void InputData()
{
	string line;
	int cur;
	int next;
	int data;

	for(int i=0; i<N; ++i)
	{
		getline(cin, line, '\n');
		istringstream stream(line);
		stream >> cur;
		stream >> data;
		stream >> next;
		q[cur].data = data;
		q[cur].next = next;

		if(next!=-1)
			q[next].prev = cur;
		else
			tail = cur;
	}
	q[head].prev = -1;
}

// 反序
void ReverseList()
{
	int rest = N % K;

	int nexthead = -1;
	int prev =  -1;
	int cur = tail;
	int next = -1;

	if(rest > 0)
	{//跳过最后几个结点
		for(int i=0; i<rest; ++i)
		{
			cur = q[cur].prev;
		}
		nexthead = q[cur].next;
	}

	for(int i=0; i<N/K; ++i)
	{
		next = nexthead;
		int revhead = cur;

		for(int j=0; j<K; ++j)
		{//子串所有元素头尾替换
			prev = q[cur].prev;
			q[cur].next = prev;
			q[cur].prev = next;

			next = cur;
			cur = prev;
		}

		int revtail = next;
		q[revtail].next = nexthead;
		q[nexthead].prev = revtail;
		q[revhead].prev=prev;
		nexthead = revhead;
	}
	head = nexthead;
}

void Print()
{
	int cur = head;
	while(cur!=-1)
	{
		cout << FORMAT << cur << "\t" << q[cur].data << "\t" << FORMAT << q[cur].prev << "\t" << FORMAT << q[cur].next << endl;
		cur = q[cur].next;
	}
}

// 汇总
void Run()
{
	
	InputHead();
	InputData();

	cout << "Origin list" <<endl;
	Print();
	cout << "Reverse list" <<endl;
	ReverseList();
	Print();
	cout << "Reverse again list" <<endl;
	ReverseList();
	Print();
}

int main()
{
	Run();
	return 0;
}
