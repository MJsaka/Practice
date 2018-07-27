#if defined _WIN32 || defined _WIN64

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#endif //_WIN32 || _WIN64

#include <iterator>
#include <iostream>
#include <stack>
using namespace std;

template <class T, class Pointer, class Reference>
struct FullPathTreeIterator : public iterator<bidirectional_iterator_tag,T,ptrdiff_t,Pointer,Reference>
{
public:
	FullPathTreeIterator(Pointer p){_p = p;};
	FullPathTreeIterator(const FullPathTreeIterator &i){_p = i._p;};

	FullPathTreeIterator& operator++()
	{
		next();
		return *this;
	}
	FullPathTreeIterator operator++(int)
	{
		FullPathTreeIterator x = *this;
		next();
		return x;
	}
	FullPathTreeIterator& operator--()
	{
		prev();
		return *this;
	}
	FullPathTreeIterator operator--(int)
	{
		FullPathTreeIterator x = *this;
		prev();
		return x;
	}
	Pointer operator-> () const
	{
		return _p;
	}

	Reference operator* () const
	{
		return *_p;
	}
	operator Pointer ()
	{
		return _p;
	}
	bool operator == (const FullPathTreeIterator& x) const
	{
		return _p == x._p;
	}
	bool operator != (const FullPathTreeIterator& x) const
	{
		return _p != x._p;
	}
protected:
	Pointer _p;
	void next()
	{
		while (_p != NULL)
		{
			if (_p->next != NULL)
			{//有右兄弟
				_p = _p->next;
				while (_p->child!=NULL)
				{//找右兄弟子树最左边的叶子
					_p = _p->child;
				}
				return ;
			}
			//没有右兄弟,往上找父结点的右兄弟
			_p = _p->parent;
		}
	}

	void prev()
	{
		while (_p != NULL)
		{
			if (_p->prev != NULL)
			{//有左兄弟
				_p = _p->prev;

				if(_p->child != NULL)
				{// 左兄弟有子树,前一个为左兄弟的最右边的叶子
					_p = _p->child;

					while (true)
					{//找左兄弟子树最右边的叶子
						if (_p->next!=NULL)
						{
							_p = _p->next;
						}else if (_p->child!=NULL)
						{
							_p = _p->child;
						}else
						{
							break;
						}
					}
				}
				return ;
			}
			//没有左兄弟,往上找父结点的右兄弟
			_p = _p->parent;
		}		
	}
};
template <class T, class Pointer, class Reference>
struct FullPathTreeReverseIterator : public FullPathTreeIterator<T,Pointer,Reference>
{
	typedef FullPathTreeIterator<T,Pointer,Reference> _Base;
public:
	FullPathTreeReverseIterator(Pointer p):_Base::FullPathTreeIterator(p){};
	FullPathTreeReverseIterator(const FullPathTreeReverseIterator &i):_Base::FullPathTreeIterator(i){};
	FullPathTreeReverseIterator& operator++()
	{
		_Base::prev();
		return *this;
	}
	FullPathTreeReverseIterator operator++(int)
	{
		FullPathTreeReverseIterator x = *this;
		_Base::prev();
		return x;
	}
	FullPathTreeReverseIterator& operator--()
	{
		_Base::next();
		return *this;
	}
	FullPathTreeReverseIterator operator--(int)
	{
		FullPathTreeReverseIterator x = *this;
		_Base::next();
		return x;
	}
};

struct _pNode
{
	_pNode  *parent, *child, *prev, *next;
	int v;

	int Value()
	{
		int i = 1;
		int val = 0;
		_pNode * n = this;
		while (n!=NULL)
		{
			val = val + n->v * i;
			n = n->parent;
			i *= 10;
		}
		return val;
	}
};

typedef void (*CallBack)(_pNode *node);

struct NumTree
{
	typedef FullPathTreeIterator<_pNode,_pNode*,_pNode&> iterator;
	typedef FullPathTreeIterator<_pNode,const _pNode*,const _pNode&> const_iterator;
	typedef FullPathTreeReverseIterator<_pNode,_pNode*,_pNode&> reverse_iterator;
	typedef FullPathTreeReverseIterator<_pNode,const _pNode*,const _pNode&> const_reverse_iterator;

	NumTree()
	{
		root = NULL;
	}
	~NumTree()
	{
		_pNode* p = root;
		_pNode* q;

		while (p!=NULL)
		{
			//先移动到最右最深
			while (true)
			{
				if (p->next != NULL)
				{
					p = p->next;
				}else if (p->child != NULL)
				{
					p = p->child;
				}else
				{
					break;
				}
			}

			q = p;
			if (q->prev!=NULL)
			{//往左查找
				p = q->prev;
				p->next = NULL;
			}else
			{//往上查找
				p = q->parent;
				if (p!=NULL)
				{
					p->child = NULL;
				}
			}
			q->prev = NULL;
			q->parent = NULL;
			delete q;
			q=NULL;
		}
	}

	const iterator begin() const
	{
		_pNode* p = root;
		while (p!=NULL && p->child != NULL)
			p = p->child;

		return iterator(p);
	}
	const iterator end() const
	{
		return iterator(NULL);
	}

	const reverse_iterator rbegin() const
	{
		_pNode* p = root;
		while (true)
		{
			if (p!=NULL&&p->next != NULL)
			{
				p = p->next;
			}else if (p!=NULL&&p->child != NULL)
			{
				p = p->child;
			}else
			{
				break;
			}
		}

		return reverse_iterator(p);
	}

	const reverse_iterator rend() const
	{
		return reverse_iterator(NULL);
	}

	void DepthFirstTravel(_pNode* root, CallBack callbackfunc)
	{		
		for (iterator iter = begin(); iter != end(); iter++)
		{
			callbackfunc(iter);
		}
	}

	void Print() const
	{
		for (iterator iter = begin(); iter != end(); iter++)
		{
			std::cout << iter->Value() << std::endl;			
		}
	}

	void PrintReverse() const
	{
		for (reverse_iterator iter = rbegin(); iter != rend(); iter++)
		{
			std::cout << iter->Value() << std::endl;			
		}
	}

	void InsetNode(int value)
	{
		_pNode* FindLeftNode = NULL;
		_pNode* FindRightNode = NULL;

		for (iterator iter = begin(); iter != end(); iter++)
		{
			int val = iter->Value();
			if (val < value)
			{
				FindLeftNode = iter;
			}else if (val == value)
			{
				FindLeftNode = FindRightNode = iter;
				break;
			}else
			{
				FindRightNode = iter;
				break;
			}
		}

		std::stack<int> vs;
		while(value>=0)
		{//先将数据入栈
			vs.push(value % 10);			
			value /= 10;
			if (value == 0)
			{
				break;
			}
		}

		std::stack<_pNode*> ql;
		std::stack<_pNode*> qr;
		//左右路径入栈
		_pNode* node = FindLeftNode;
		while (node !=NULL)
		{
			ql.push(node);
			node = node->parent;
		}
		node = FindRightNode;
		while (node != NULL)
		{
			qr.push(node);
			node = node->parent;
		}
		_pNode* parent = NULL;
		_pNode* prev = NULL;
		_pNode* next = NULL;

		if (FindLeftNode == FindRightNode && FindLeftNode != NULL)
		{//数据已存在
			return ;
		}else if (FindLeftNode==FindRightNode && FindLeftNode==NULL)
		{//还未插入数据

		}else
		{
			while(!ql.empty() && !qr.empty() && ql.top() == qr.top())
			{//如果有公共部分,则新数据也会与之是公共部分
				parent = ql.top();
				ql.pop();
				qr.pop();
				vs.pop();
			}
			if (!ql.empty() && ql.top()->child != NULL && vs.top() == ql.top()->v)
			{//若左子树不为空,新插入的数据比左子树大,如果与左子树有公共部分,则先找与左子树的公共部分
				do {
					parent = ql.top();
					vs.pop();
					ql.pop();
				}while (!ql.empty() && ql.top()->child != NULL && vs.top() == ql.top()->v);

				prev = ql.top();
				next = NULL;
			}else  if (!qr.empty() && qr.top()->child != NULL && vs.size() > 1 && vs.top() == qr.top()->v)
			{//若右子树不为空,新插入的数据比右子树小,如果与右子树有公共部分,先找与右子树的公共部分
				//若比右子树小,有可能长度比右子树短,此时,应该在其前面插入一个新结点
				//如插入111之后,再插入11
				do {
					parent = qr.top();
					vs.pop();
					qr.pop();
				}while (!qr.empty() && qr.top()->child != NULL  && vs.size() > 1 && vs.top() == qr.top()->v);

				next = qr.top();
				prev = NULL;
			} else 
			{//与左右子树没有更多的公共部分,应该在中间插入新结点
				if (!ql.empty())
				{
					prev = ql.top();
				}

				if (!qr.empty())
				{
					next = qr.top();
				}
			}
		}

		node = new _pNode;
		node->v = vs.top();
		node->child = NULL;
		node->prev = prev;
		node->next = next;
		if (prev != NULL)
		{
			prev->next = node;
		}
		if (next != NULL)
		{
			next->prev = node;
		}

		node->parent = parent;

		if (prev == NULL)
		{
			if (parent!=NULL)
			{
				parent->child = node;
			}else
			{
				root = node;
			}
		}
		parent = node;
		vs.pop();

		while (!vs.empty())
		{
			node = new _pNode;
			node->v = vs.top();
			node->child = NULL;
			node->prev = NULL;
			node->next = NULL;
			node->parent = parent;
			parent->child = node;
			parent = node;

			vs.pop();
		}
	}

	_pNode* root;
};

int main(int argc, char* argv[])
{
#if defined _WIN32 || defined _WIN64
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif

	NumTree tree;

	int a[] = {111,22,11,1,3,2,666,7,123,112,124,156};

	for (int i=0 ; i < sizeof(a) / sizeof(int); ++i)
	{
		tree.InsetNode(a[i]);
	}

	tree.Print();
	cout << endl;
	tree.PrintReverse();

	getchar();
	return 0;
}
