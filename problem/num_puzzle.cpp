#include <vector>
#include <list>
#include <deque>
#include <set>
#include <string>
#include <iostream>
#include <sstream>
#include <memory>

#include <time.h>
#include <stdlib.h>

using namespace std;

static const int MoveLeft = 0;
static const int MoveRight = 1;
static const int MoveUp = 2;
static const int MoveDown = 3;
static const string DirStr[4] = {"<",">","^","v"};
static const int RDir[4] = {1,0,3,2};
static const int DirXY[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};

class Puzzle {
	public:
		// 初始化
		Puzzle(vector<vector<int>> &array) {
			m_array = array;
			m_row = m_array.size();
			m_col = m_array[0].size();

			getXY();

			m_pathList.clear();
		}

		//默认初始化
		Puzzle() {
			randomArray(3,3);
			m_row = m_array.size();
			m_col = m_array[0].size();

			getXY();

			m_pathList.clear();
		}

		// 判断是否有可行路径
		bool canSolve()
		{
			return solveBreadth();
		}
		// 判断是否胜利
		bool win()
		{
			for(int k=0; k<m_row*m_col-1; k++)
			{
				int i=k/m_col;
				int j=k%m_col;
				if(m_array[i][j] != k + 1)
					return false;
			}

			return true;
		}

		// 判断是否可以朝某个方向进行移动
		bool canMove(int dir) {
			switch (dir) {
				case MoveLeft:
					return m_x > 0;
				case MoveRight:
					return m_x < m_col-1;
				case MoveUp:
					return m_y > 0;
				case MoveDown:
					return m_y < m_row-1;
			}
			return false;
		}
		void move(int dir) 
		{
			int x = m_x + DirXY[dir][0];
			int y = m_y + DirXY[dir][1];
			std::swap(m_array[m_y][m_x],m_array[y][x]);
			m_x = x;
			m_y = y;
		}
		// 朝某个方向进行移动，该函数不作判断，直接移动
		void moveForward(int dir) {
			move(dir);
			m_pathList.push_back(dir);
#ifdef DEBUG
			cout << "move " << getDirString(dir) << " " << getStatus() << endl;
#endif
		}

		// 某个方向的回退，该函数不作判断，直接移动
		void moveBackward(int dir) {
			move(RDir[dir]);
			m_pathList.pop_back();
#ifdef DEBUG
			cout << "back " << getDirString(dir) << " " << getStatus() << endl;
#endif
		}

		// 打印当前的状态
		void print() {
			for(int i=0; i<m_row; i++) {
				for(int j=0; j<m_col; j++) {
					cout << m_array[i][j];
				}
				cout << endl;
			}
		}

		// 获取提示路径 使用前必须先判断是否有可行路径
		list<int> getHintPath()
		{
			list<int> path;
			shared_ptr<SearchStep> step = m_winStep;
			while(step->getDir() != -1) {
				path.push_front(step->getDir());
				step = step->getParent();
			}
			return path;
		}
		// 打印移动路径
		void printRoute() {
			printPath(m_pathList);
		}

		// 根据链表找到路径
		void printHintRoute() {
			printPath(getHintPath());
		}
		// 打印路径
		void printPath(const list<int>& path)
		{
			for(auto iter=path.begin(); iter!=path.end(); iter++)
			{
				cout << getDirString(*iter) << " ";
			}
			cout << endl;
		}

		// 方向与其对应的字符串
		string getDirString(int dir) {
			return DirStr[dir];
		}

	private:

		// 游戏数据
		vector<vector<int>> m_array;
		// 行列数
		int m_row;
		int m_col;

		// 空格的位置
		int m_x;
		int m_y;

		// 移动路径
		list<int> m_pathList;
		// 保存已经搜索过的状态
		set<string> m_statusSet;

		// 广搜每一步状态，通过parent链起来
		class SearchStep {
			string status;
			int dir;
			shared_ptr<SearchStep> parent;
			public:
			SearchStep(string status, int dir, shared_ptr<SearchStep> parent) {
				this->status = status;
				this->dir = dir;
				this->parent = parent;
			}
			string getStatus() {return status;}
			int getDir() {return dir;}
			shared_ptr<SearchStep> getParent() {return parent;}
		};
		//广搜队列
		deque<shared_ptr<SearchStep>> m_searchQueue;
		//可行路径最后一步
		shared_ptr<SearchStep> m_winStep;
		//初始化
		void randomArray(int row, int col)
		{
			// 初始状态设为胜利状态
			m_array.clear();
			m_array.resize(row);
			for(int i=0; i<row; i++) {
				m_array[i].resize(col);
				for(int j=0; j<col; j++)
					m_array[i][j] = i*col+j+1;
			}
			m_array[row-1][col-1] = 0;

			//随机交换两个数字偶数次
			srand(time(NULL));
			int k=0;
			while(k<1000)
			{
				int x1 = rand() % col;
				int y1 = rand() % row;
				int x2 = rand() % col;
				int y2 = rand() % row;
				if(x1!=x2&&y1!=y2)
				{
					k++;
					swap(m_array[y1][x1],m_array[y2][x2]);
				}
			}

			//用逆序数判断是否有解
			int cnt = 0;
			for(int i=0; i<row*col-1; i++) {
				for(int j=i+1; j<row*col; j++) {
					if(m_array[i/col][i%col] > m_array[j/col][j%col])
						cnt++;
				}
			}

			int x,y;
			for(int i=0; i<row; i++) {
				for(int j=0; j<col; j++) {
					if(m_array[i][j] == 0) {
						y = i;
						x = j;
						break;
					}
				}
			}

			if(cnt%2 != (x+y)%2)
				swap(m_array[0][0],m_array[0][1]);
		}

		//获取空白位置
		void getXY()
		{
			for(int i=0; i<m_row; i++) {
				for(int j=0; j<m_col; j++) {
					if(m_array[i][j] == 0) {
						m_y = i;
						m_x = j;
						break;
					}
				}
			}
		}

		// 获取状态
		string getStatus()
		{
			stringstream ss;
			for (int i=0; i<m_row; i++)
			{
				for (int j=0; j<m_col; j++)
				{
					ss << m_array[i][j] << " ";
				}
			}
			return ss.str();
		}

		// 根据状态恢复
		void recoverStatus(string s)
		{
			stringstream ss(s);
			for (int i=0; i<m_row; i++)
			{
				for (int j=0; j<m_col; j++)
				{
					ss >> m_array[i][j];
				}
			}
			getXY();
		}

		// 深度优先搜索
		bool searchDeep(int dir) {
			// 如果能够朝该方向行走
			if(canMove(dir)) {
				// 往该方向移动
				moveForward(dir);
				// 移动后的状态
				string status = getStatus();
				// 如果已经是胜利状态，返回true
				if(win()) {
					return true;
				}
				// 如果是之前走过的状态了，返回false
				if(m_statusSet.find(status) != m_statusSet.end()) {
					// 这一步走错了，回退
					moveBackward(dir);
					return false;
				}
				//当前路径步数太多
				if(m_pathList.size()>100) {
					moveBackward(dir);
					return false;
				}
				// 将当前状态存入set
				m_statusSet.insert(status);
				// 继续朝四个方向进行搜索
				bool searchFourOk = searchDeep(MoveRight) || searchDeep(MoveDown) || searchDeep(MoveLeft) || searchDeep(MoveUp);
				if(searchFourOk) {
					return true;
				} else {
					// 这一步走错了，把它的记录去除
					moveBackward(dir);
					return false;
				}
			}
			return false;
		}

		// 深度优先
		bool solveDeep() {
			//清理状态
			m_statusSet.clear();
			m_searchQueue.clear();

			// 如果已经是胜利状态，返回true
			if(win()) {
				return true;
			}
			// 初始状态先记录
			m_statusSet.insert(getStatus());
			// 朝4个方向进行搜索
			return searchDeep(MoveRight) || searchDeep(MoveDown) || searchDeep(MoveLeft) || searchDeep(MoveUp);
		}

		// 广度优先搜索
		bool searchBreadth() {
			// 如果还有要搜索的状态
			while(m_searchQueue.size() > 0) {
				// 队首出列
				shared_ptr<SearchStep> step = m_searchQueue.front();
				m_searchQueue.pop_front();

				// 根据status还原arr和x，y
				string status = step->getStatus();
				recoverStatus(status);
#ifdef DEBUG
				cout << endl << "status " << status << endl;
#endif
				// 找到路径
				if(win()) {
					m_winStep = step;
					return true;
				}
				// 4个方向进行遍历
				for(int i=0; i<4; i++) {
					// 如果能够朝该方向行走
					if(canMove(i)) {
						// 向前一步
						moveForward(i);
						status = getStatus();
						// 之前搜过的状态
						if (m_statusSet.find(status) != m_statusSet.end()) {
							moveBackward(i);
							continue;
						}
						// 新状态加入待搜索
						m_statusSet.insert(status);
						m_searchQueue.push_back(
								make_shared<SearchStep>(
									SearchStep(status, i, step) ) );
						moveBackward(i);
					}//if
				}//for
			}//while
			return false;
		}

		// 广度优先
		bool solveBreadth() {
			//清理状态
			m_statusSet.clear();
			m_searchQueue.clear();

			// 如果已经是胜利状态，返回true
			if(win()) {
				return true;
			}
			string status = getStatus();
			// 初始状态先记录
			m_statusSet.insert(status);
			// 初始状态进入搜索队列
			m_searchQueue.push_back(
					make_shared<SearchStep>(
						SearchStep( status, -1, shared_ptr<SearchStep>() ) ) );
			bool r = searchBreadth();
			recoverStatus(status);

			return r;
		}
};

class Game
{
	Puzzle m_puzzle;

	public:

	void newGame()
	{
		cout << "-------------New Game-------------" << endl;
		m_puzzle = Puzzle();
		m_puzzle.print();
	}
	void help()
	{
		cout << "==================================" << endl;
		cout << "======    NumPuzzle Game    ======" << endl;
		cout << "==================================" << endl;
		cout << "move the blank to solve the puzzle" << endl;
		cout << "q   : quit" << endl;
		cout << "h   : help" << endl;
		cout << "r   : newgame" << endl;
		cout << "t   : hint" << endl;
		cout << "g   : autorun" << endl;
		cout << "a j : move left" << endl;
		cout << "d l : move right" << endl;
		cout << "w i : move up" << endl;
		cout << "s k : move down" << endl;
	}
	//提示是否有可行路径
	void hint()
	{
		if(m_puzzle.canSolve())
		{
			cout << "can solve:";
			m_puzzle.printHintRoute();
		}else
		{
			cout << "cann't solve" << endl;
		}
	}
	//自动按照可行路径移动
	void autorun()
	{
		if(m_puzzle.canSolve())
		{
			list<int> path = m_puzzle.getHintPath();
			while(!path.empty())
			{
				m_puzzle.printPath(path);
				move(path.front());
				path.pop_front();
			}
		}else
		{
			cout << "cann't solve" << endl;
		}
	}

	void move(int dir)
	{
		if(m_puzzle.win())
			return;

		if(m_puzzle.canMove(dir))
		{
			m_puzzle.moveForward(dir);
			m_puzzle.print();
		}
		if(m_puzzle.win())
		{
			cout << "win! Route:" << endl;
			m_puzzle.printRoute();
		}
	}
	void run()
	{
		char c;
		while(cin>>c)
		{
			switch(c)
			{
				case 'a': case 'j':
					move(MoveLeft);
					break;
				case 'd': case 'l':
					move(MoveRight);
					break;
				case 'w': case 'i':
					move(MoveUp);
					break;
				case 's': case 'k':
					move(MoveDown);
					break;
				case 'h':
					help();
					break;
				case 'r':
					newGame();
					break;
				case 't':
					hint();
					break;
				case 'g':
					autorun();
					break;
				case 'q':
					return;
			}
		}
	}
};

int main()
{
	Game game;
	game.help();
	game.newGame();
	game.run();
	
	return 0;
}
