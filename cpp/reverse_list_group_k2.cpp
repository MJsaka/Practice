#include <iostream>
using namespace std;

struct ListNode {
	int val;
	ListNode *next;
	ListNode(int x) : val(x), next(NULL) {}
	~ListNode()
	{
		if(next!=NULL)
			delete next;
	}
};

void print(ListNode *head)
{
	while(head!=NULL)
	{
		cout << head << " : " << head->val << " --> " << head->next << endl;
		head = head->next;
	}
};
ListNode* reverse_group_k(ListNode* head, int k)
{
	if(k<=1 || head==NULL)
		return head;

	ListNode *pre, *cur, *next;
	ListNode *prehead, *pretail;

	cur = head;
	pretail = NULL;

	while(cur!=NULL)
	{
		int i = 1;
		prehead = cur;
		pre = cur;
		cur = pre->next;

		while(cur!=NULL && i<k)
		{
			//逆序当前子串
			next = cur->next;

			cur->next = pre;
			i++;

			pre = cur;
			cur = next;
		}

		if(i==k)
		{   //此时pre为当前正子串尾(当前逆子串头),cur为下一正子串头(可能是NULL)
			if(pretail!=NULL)
			{//前一个逆子串尾指向当前逆子串头
				pretail->next = pre;
			}else
			{//当前为第一个逆子串,当前逆子串头即为head
				head = pre;
			}
			//当前正子串头变为当前逆子串尾
			pretail = prehead;
			//当前逆子串尾指向下一个正子串头,这样在逆序的中间过程,链表也是完整的
			//如果结点数正好是k的倍数,此时cur为NULL,尾结点指向NULL
			pretail->next = cur;
		}else
		{
			//此时pre为当前正子串尾(当前逆子串头),cur为NULL,将当前逆子串再次逆序
			cur = pre;
			pre = NULL;
			while(cur!=prehead)
			{
				next = cur->next;

				cur->next = pre;

				pre = cur;
				cur = next;
			}
			cur = NULL;
		}
		cout << "In progress" << endl;
		print(head);
	}
	return head;
}

void test(int n, int k)
{
	struct ListNode* head = NULL;
	struct ListNode* cur = NULL;

	for(int i=1; i<=n; ++i)
	{
		struct ListNode *node = new ListNode(i);
		if(head == NULL)
			head = node;
		else 
			cur->next = node;
		cur = node;
	}

	cout << "origin list" << endl;
	print(head);

	head = reverse_group_k(head, k);

	cout << "reverse list" << endl;
	print(head);

	delete head;
}

int main()
{
	while(true)
	{
		int n,k;
		cout << "input N:";
		cin >> n;
		cout << "input K:";
		cin >> k;
		test(n,k);
	}
}
