#include <iostream>

using namespace std;

int main()
{
	int col,row;
	cout << "input col" << endl;
	cin >> col;
	cout << "input row" << endl;
	cin >> row;

	int *buffer = new int[col*row];

	int left = 0;
	int right = col - 1;
	int top = 0;
	int bottom = row - 1;

	int i = top;
	int j = left;

	//0 left to right; 1 top to bottom; 2 right to left; 3 bottom to top;
	int dir = 0;

	for(int k = 1 ; k <= col * row; ++k)
	{
		buffer[i*col+j] = k;
		switch(dir)
		{
			case 0:
				j++;
				if(j==right)
				{
					dir = (dir + 1)%4;
					top++;
				}
				break;
			case 1:
				i++;
				if(i==bottom)
				{
					dir = (dir + 1)%4;
					right--;
				}
				break;
			case 2:
				j--;
				if(j==left)
				{
					dir = (dir + 1)%4;
					bottom--;
				}
				break;
			case 3:
				i--;
				if(i==top)
				{
					dir = (dir + 1)%4;
					left++;
				}
				break;
		}
	}

	for(int m = 0; m < row; ++m)
	{
		for(int n = 0; n < col; ++n)
		{
			cout << buffer[m*col+n] << '\t';
		}
		cout << endl;
	}
	delete [] buffer;
}
