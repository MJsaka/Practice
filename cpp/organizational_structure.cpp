#include <iostream>
#include <string>
#include <list>

using namespace std;

const int show_status = 1;

template<class T>
class MTree
{
	T m_data;
	MTree<T>* m_pParent;
	list<MTree<T>*> m_pChildren;

public:
	MTree(T data)
	{
		m_data = data;
		m_pParent = NULL;
	}

	~MTree()
	{
		for(auto iter = m_pChildren.begin(); iter != m_pChildren.end(); iter++)
		{
			delete (*iter);
		}
	}

	MTree<T> * findChild(T data)
	{
		if(m_data == data)
		{
			if(show_status)	cout << data << " <-- ";
			return this;
		}
		MTree<T> *node;
		for(auto iter = m_pChildren.begin(); iter != m_pChildren.end(); iter++)
		{
			if(node = (*iter)->findChild(data))
			{
				if(show_status)	cout << m_data << " <-- ";
				return node;
			}
		}
		return NULL;
	}

	bool insertChild(MTree<T> *node)
	{
		if(node->m_pParent == this)
		{
			if(show_status)	cout << "parent " << m_data <<  " has child " << node->m_data << endl; 
			return false;
		}
		if(node->findChild(m_data))
		{
			if(show_status)	cout << m_data << endl << "*** error: child insert parent as child ***" << endl;
			return false;
		}

		node->changeParent(this);

		m_pChildren.push_back(node);
		if(show_status)	cout << "parent " << m_data <<  " add child " << node->m_data << endl; 

		return true;
	}
	bool removeChild(MTree<T> *node)
	{
		for(auto iter = m_pChildren.begin(); iter != m_pChildren.end(); iter++)
		{
			if(*iter == node)
			{
				if(show_status)	cout << "parent " << m_data << " remove child " << node->m_data << endl;
				m_pChildren.erase(iter);
				return true;
			}
		}

		return false;
	}
	void changeParent(MTree<T> *parent)
	{
		if(m_pParent)
		{
			m_pParent->removeChild(this);
		}
		m_pParent = parent;
	}

	void printChildren(string prefix)
	{
		int size = m_pChildren.size();
		if( size > 0 )
		{
			for(auto iter = m_pChildren.begin(); iter != m_pChildren.end(); iter++)
			{
				size --;
				if(size > 0)
					(*iter)->print(prefix);
				else
					(*iter)->print(prefix, true);
			}
		}
	}
	void print(string prefix, bool last = false)
	{
		cout << prefix << (last ? "└── " : "├── ") << m_data << endl;
		prefix = prefix + (last ? "    " : "│   ");

		printChildren(prefix);
	}

	void printRoot()
	{
		cout << m_data << endl;
		printChildren("");
	}
};

#define MATCH(a,b) if((a) != (b)){ cout << "input error" << endl; exit(-1); }
#define MATCHNOT(a,b) if((a) == (b)){ cout << "input error" << endl; exit(-1); }

void string2data(const string& s, int& data)
{
	data = atoi(s.c_str());
}

void string2data(const string& s, string& data)
{
	data = s;
}

template <class T>
bool nextToken(T& data1, T& data2)
{
	string s1,s2;
	char c;
	do {
		cin >> c;
	} while (c == ' ' || c == '\t' || c == '\n');

	MATCH(c, '{');

	do {
		cin >> c;
	} while (c == ' ' || c == '\t' || c == '\n');

	MATCHNOT(c, '{');
	MATCHNOT(c, ',');
	MATCHNOT(c, '}');
	MATCHNOT(c, ';');

	do {
		s1.insert(s1.end(),c);
		cin >> c;
	} while (c != ' ' && c != '\t' && c != '\n' &&
		 c != '{' && c != ',' && c != '}' && c!= ';');

	while (c == ' ' || c == '\t' || c == '\n')
	{
		cin >> c;
	}

	MATCH(c, ',');

	do {
		cin >> c;
	} while (c == ' ' || c == '\t' || c == '\n');

	MATCHNOT(c, '{');
	MATCHNOT(c, ',');
	MATCHNOT(c, '}');
	MATCHNOT(c, ';');

	do {
		s2.insert(s2.end(),c);
		cin >> c;
	} while (c != ' ' && c != '\t' && c != '\n' &&
		 c != '{' && c != ',' && c != '}' && c!= ';');

	while (c == ' ' || c == '\t' || c == '\n')
	{
		cin >> c;
	}

	MATCH(c, '}');

	string2data(s1,data1);
	string2data(s2,data2);

	do {
		cin >> c;
	} while (c == ' ' || c == '\t' || c == '\n');

	if (c == ';')
		return true;
	else {
		MATCH(c, ',');
		return false;
	}
}

template<class T>
void test()
{
	MTree<T> *root = new MTree<T>(T());	

	cout << "input data like \"{ data , data } [ , { data , data } ]... ;\"" << endl;
	while(true)
	{
		char c;
		T employee,leader;

		bool eof = nextToken(employee, leader);

		if(show_status)	cout << "{ " << employee << " " << leader << " }" <<endl;
		MTree<T>* node_leader = root->findChild(leader);
		if(!node_leader)
		{
			if(show_status)	cout << "new leader " << leader << endl;
			node_leader = new MTree<T>(leader);
			root->insertChild(node_leader);
		}else
		{
			if(show_status)	cout << "find leader" << endl;
		}
		MTree<T>* node_employee = root->findChild(employee);
		if(!node_employee)
		{
			if(show_status)	cout << "new employee " << employee << endl;
			node_employee= new MTree<T>(employee);
		}else
		{
			if(show_status)	cout << "find employee" << endl;
		}
		node_leader->insertChild(node_employee);
		if(show_status)	root->printRoot();

		if(eof) break;
	}

	root->printRoot();
	delete root;
}

int main() 
{
	cout << "test int" <<endl;
	test<int>();

	cout << "test string" <<endl;
	test<string>();
}
