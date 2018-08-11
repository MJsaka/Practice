// Russian_cube.cpp : 定义应用程序的入口点。
//
//
//
//
#include "stdafx.h"
#include <atlstr.h>
#include "Russian_cube.h"

#define MAX_LOADSTRING 100
//Tetris
#define BOUND_SIZE 10
#define TETRIS_SIZE 30
#define GAME_X 10
#define GAME_Y 20
#define INFO_X 6
#define INFO_Y GAME_Y

//定时器
#define  MY_TIMEER 1
#define  DEFAULT_INTERVAL 500 //默认每0.5秒下降一格

//定义俄罗斯方块的形状
UINT g_astTetris[7][4][4][4] =
{
	{
		{
			{ 1, 1, 0, 0 }, 
			{ 1, 1, 0, 0 }, 
			{ 0, 0, 0, 0 }, // ■■
			{ 0, 0, 0, 0 }  // ■■
		},
	},

	{
		{
			{ 1, 1, 1, 1 }, 
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 } //■■■■
		},
		{
			{ 1, 0, 0, 0 }, //■
			{ 1, 0, 0, 0 }, //■
			{ 1, 0, 0, 0 }, //■
			{ 1, 0, 0, 0 }  //■
		},
	},

	{
		{
			{ 0, 1, 1, 0 }, // ■■
			{ 1, 1, 0, 0 }, //■■
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 1, 0, 0, 0 }, //■
			{ 1, 1, 0, 0 }, //■■
			{ 0, 1, 0, 0 }, // ■
			{ 0, 0, 0, 0 }
		}
	},
	{
		{
			{ 1, 1, 0, 0 }, //■■
			{ 0, 1, 1, 0 }, // ■■
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 0, 0 }, // ■
			{ 1, 1, 0, 0 }, //■■
			{ 1, 0, 0, 0 },  //■
			{ 0, 0, 0, 0 }
		}
	},
	{
		{ 
			{ 1, 0, 0, 0 }, //■
			{ 1, 0, 0, 0 }, //■
			{ 1, 1, 0, 0 }, //■■
			{ 0, 0, 0, 0 } 
		},
		{
			{ 1, 1, 1, 0 }, //■■■
			{ 1, 0, 0, 0 }, //■
			{ 0, 0, 0, 0 }, 
			{ 0, 0, 0, 0 },
		},
		{
			{ 1, 1, 0, 0 }, //■■
			{ 0, 1, 0, 0 }, // ■
			{ 0, 1, 0, 0 }, // ■
			{ 0, 0, 0, 0 } 
		},
		{
			{ 0, 0, 1, 0 }, // ■
			{ 1, 1, 1, 0 }, //■■■
			{ 0, 0, 0, 0 }, 
			{ 0, 0, 0, 0 } 
		}
	},
	{
		{
			{ 1, 1, 0, 0 }, //■■
			{ 1, 0, 0, 0 }, //■ 
			{ 1, 0, 0, 0 }, //■ 
			{ 0, 0, 0, 0 }
		},
		{
			{ 1, 1, 1, 0 }, //■■■
			{ 0, 0, 1, 0 }, //  ■
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 0, 0 }, // ■
			{ 0, 1, 0, 0 }, // ■
			{ 1, 1, 0, 0 }, // ■■
			{ 0, 0, 0, 0 }
		},
		{
			{ 1, 0, 0, 0 }, //■ 
			{ 1, 1, 1, 0 }, //■■■
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		}
	},
	{
		{
			{ 0, 1, 0, 0 }, // ■
			{ 1, 1, 1, 0 }, //■■■
			{ 0, 0, 0, 0 }, 
			{ 0, 0, 0, 0 }
		},
		{ 
			{ 1, 0, 0, 0 }, //■
			{ 1, 1, 0, 0 }, //■■
			{ 1, 0, 0, 0 }, //■
			{ 0, 0, 0, 0 }
		},
		{ 
			{ 1, 1, 1, 0 }, //■■■
			{ 0, 1, 0, 0 }, // ■
			{ 0, 0, 0, 0 }, 
			{ 0, 0, 0, 0 }
		}, 
		{
			{ 0, 1, 0, 0 }, // ■
			{ 1, 1, 0, 0 }, //■■
			{ 0, 1, 0, 0 }, // ■
			{ 0, 0, 0, 0 }
		}
	},
};
UINT g_astTetrisHeight[7][4] = 
{
	{2},
	{1,4},
	{2,3},
	{2,3},
	{3,2,3,2},
	{3,2,3,2},
	{2,3,2,3},	
};
#define  TETRIS_CNT 7

UINT g_TetrisModeCount[TETRIS_CNT] = { 1, 2, 2, 2, 4, 4, 4};
COLORREF g_TetrisColor[TETRIS_CNT] = {
	RGB(0xc0,0,0),
	RGB(0,0xc0,0),
	RGB(0,0,0xc0),
	RGB(0xc0,0xc0,0),
	RGB(0xc0,0,0xc0),
	RGB(0,0xc0,0xc0),
	RGB(0xc0,0xc0,0xc0)
};

HBRUSH g_TetrisBrush[TETRIS_CNT];
HPEN g_TetrisPen[TETRIS_CNT];

BOOL g_GameRunning = FALSE;
BOOL g_GameOver = FALSE;
//当前方块的形状
UINT g_CurIndex[2];
UINT g_NextIndex[2];
UINT g_CurTetris[4][4];
UINT g_NextTetris[4][4];
UINT g_stGame[GAME_Y][GAME_X];//记录已经落下来的方块
UINT g_stGameType[GAME_Y][GAME_X];//记录已经落下来的方块

UINT g_Score[4] = {1,2,5,10};
UINT g_ToAnimateLine[4];

//记录方块左上角的坐标
INT TetrisX;
INT TetrixY;
UINT g_uiInterval;
UINT g_uiScore;

UINT g_uiMySeed = 0xffff;

UINT g_DownSpeed = 0; //方块下落速度,按下方向键的下键时加速落下

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

HDC         hdcMem;
HBITMAP     bmpMem;

HWND g_hWnd;

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int GetRandNum(int iMin,int iMax);
VOID RotateTetris(UINT bTetris[4][4]);
BOOL CheckTetris(int nStartX,int nStartY,UINT bTetris[4][4],UINT bGame[GAME_Y][GAME_X]);
VOID RefreshTetris(int nStartX,int nStartY,UINT bTetris[4][4], UINT bGame[GAME_Y][GAME_X]);
VOID DrawBackGround(HDC hdc);
VOID DrawInfo(HDC hdc);
VOID DrawTetris(HDC hdc, int nStartX,int nStartY,UINT bTetris[4][4]);
VOID Paint(HWND hWnd, HDC hdc, BOOL animate=FALSE);
VOID DrawNormal(HDC hdc, LPRECT pr);
VOID DrawAnimate(HDC hdc, int Duaring, int Part);
VOID Animate();

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_RUSSIAN_CUBE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RUSSIAN_CUBE));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RUSSIAN_CUBE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_RUSSIAN_CUBE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // 将实例句柄存储在全局变量中

	hWnd = CreateWindow(szWindowClass, szTitle,  WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	g_hWnd = hWnd;

	return TRUE;
}

int GetRandNum(int iMin,int iMax)
{
	//取随机数
	srand(GetTickCount() + g_uiMySeed-- );

	return iMin + rand()%(iMax -iMin);
}


//初始化游戏,就是方块最先初始化的位置
VOID InitGame()
{
	int iTmp1,iTmp2;

	TetrisX = (GAME_X - 4 )/2 ; //居中
	TetrixY = 0;

	g_uiScore = 0;
	g_uiInterval = DEFAULT_INTERVAL;

	iTmp1 = GetRandNum(0,TETRIS_CNT);
	iTmp2 = GetRandNum(0,g_TetrisModeCount[iTmp1]);
	g_CurIndex[0] = iTmp1;
	g_CurIndex[1] = iTmp2;
	memcpy(g_CurTetris,g_astTetris[iTmp1][iTmp2],sizeof(g_CurTetris));

	iTmp1 = GetRandNum(0,TETRIS_CNT);
	iTmp2 = GetRandNum(0,g_TetrisModeCount[iTmp1]);
	g_NextIndex[0] = iTmp1;
	g_NextIndex[1] = iTmp2;
	memcpy(g_NextTetris,g_astTetris[iTmp1][iTmp2],sizeof(g_CurTetris));

	memset(g_stGame,0,sizeof(g_stGame));

	g_GameOver = FALSE;
}
//旋转方块, 并且靠左上角
VOID RotateTetris(UINT bTetris[4][4])
{
	UINT iTmp1,iTmp2;
	iTmp1 = g_CurIndex[0];
	iTmp2 = (g_CurIndex[1]+1)%g_TetrisModeCount[iTmp1];
	memcpy(bTetris,g_astTetris[iTmp1][iTmp2],sizeof(g_CurTetris));
	return;
}
BOOL CheckTetris(int nStartX,int nStartY,UINT bTetris[4][4],UINT bGame[GAME_Y][GAME_X])
{
	int x,y;

	for (y = 0;y < 4;y++)
	{
		for (x = 0;x < 4;x++)
		{
			if (bTetris[y][x])
			{
				if (nStartX + x < 0)
				{//碰到左墙
					CString s1;
					s1.Format(_T("碰到左墙 %d %d %d %d \r\n"),y,x,nStartY,nStartX);
					OutputDebugString(s1);
					return FALSE;
				}
				//碰右墙
				if (nStartX + x >=GAME_X)
				{
					CString s1;
					s1.Format(_T("碰到右墙 %d %d %d %d \r\n"),y,x,nStartY,nStartX);
					OutputDebugString(s1);
					return FALSE;
				}
				//碰下墙
				if (nStartY + y >=GAME_Y)
				{
					CString s1;
					s1.Format(_T("碰到下墙 %d %d %d %d \r\n"),y,x,nStartY,nStartX);
					OutputDebugString(s1);
					return FALSE;
				}
				//碰到已有的方块
				if (nStartY + y >=0 && nStartX + x >=0 && bGame[nStartY + y][nStartX + x])
				{
					CString s1;
					s1.Format(_T("碰到已有的方块 %d %d %d %d \r\n"),y,x,nStartY,nStartX);
					OutputDebugString(s1);
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

//落地的方块合并，并且满足消除一行
VOID RefreshTetris(int nStartX,int nStartY,UINT bTetris[4][4], UINT bGame[GAME_Y][GAME_X])
{
	BOOL bFull = FALSE;
	int x,y;
	int newY;//主要用来记录
	int iFulllCount = 0; //校区满行的格子记录行数，用于积分

	for (y = 0; y < 4;y ++)
	{
		for (x = 0 ;x < 4; x++)
		{
			if (bTetris[y][x] && nStartY + y >=0 && nStartX + x >=0)
			{
				bGame[nStartY + y][nStartX + x] = TRUE;
				g_stGameType[nStartY + y][nStartX + x] = g_CurIndex[0];
			}
		}
	}

	for (int i=0; i < 4; i++)
	{
		g_ToAnimateLine[i] = -1;
	}
	UINT bToAnimate = 0;
	for (y = GAME_Y-1; y >= 0; y--)
	{
		bFull= TRUE;
		for (x = 0; x < GAME_X; x++)
		{
			if (!bGame[y][x])//这一行不满格
			{
				bFull = FALSE;
			}
		}
		if (bFull)
		{
			g_ToAnimateLine[bToAnimate] = y;
			bToAnimate++;
		}
	}

	if (bToAnimate > 0)
	{
		Animate();
	}

	for (y = GAME_Y-1, newY = GAME_Y-1; y >= 0; y--)
	{
		bFull= TRUE;
		for (x = 0; x < GAME_X; x++)
		{
			bGame[newY][x] = bGame[y][x];
			g_stGameType[newY][x] = g_stGameType[y][x];
			if (!bGame[y][x])//这一行不满格
			{
				bFull = FALSE;
			}
		}
		if (bFull)
		{
			//满格的话，用上一行替换这一行
			iFulllCount++;
		}
		else
		{
			newY--;
		}
	}

	if (iFulllCount)
	{
		g_uiScore += g_Score[iFulllCount-1];
	}
	//合并以后生成新的方块，并刷新位置
	g_CurIndex[0] = g_NextIndex[0];
	g_CurIndex[1] = g_NextIndex[1];
	memcpy(g_CurTetris,g_NextTetris,sizeof(g_CurTetris));

	int iTmp1,iTmp2;
	iTmp1 = GetRandNum(0,TETRIS_CNT);
	iTmp2 = GetRandNum(0,g_TetrisModeCount[iTmp1]);
	g_NextIndex[0] = iTmp1;
	g_NextIndex[1] = iTmp2;
	memcpy(g_NextTetris,g_astTetris[iTmp1][iTmp2],sizeof(g_CurTetris));

	TetrisX = (GAME_X - 4)/2;
	int h = g_astTetrisHeight[g_CurIndex[0]][g_CurIndex[1]];
	TetrixY = 1-h;

	CString s1;
	s1.Format(_T("Y % -3d % -3d % -3d\r\n"),TetrixY,g_CurIndex[0],g_CurIndex[1]);
	OutputDebugString(s1);
}

VOID DrawBackGround(HDC hdc)
{
	int x, y;
	HPEN hPen = (HPEN)GetStockObject(NULL_PEN);
	HBRUSH hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);

	SelectObject(hdc,hPen);

	Rectangle(hdc,BOUND_SIZE,BOUND_SIZE,BOUND_SIZE + GAME_X*TETRIS_SIZE
		,BOUND_SIZE + GAME_Y * TETRIS_SIZE);
	
	RECT r;

	for (y = 0 ;y < GAME_Y; y++)
	{
		for (x = 0; x < GAME_X; x++)
		{
			SetRect(&r,BOUND_SIZE + x*TETRIS_SIZE,
				BOUND_SIZE + y * TETRIS_SIZE,
				BOUND_SIZE + (x + 1)*TETRIS_SIZE,
				BOUND_SIZE + (y + 1) * TETRIS_SIZE);

			if (g_stGame[y][x])
			{
				SelectObject(hdc,g_TetrisBrush[g_stGameType[y][x]]);
				SelectObject(hdc,GetStockObject(BLACK_PEN));

				Rectangle(hdc,r.left,r.top,r.right,r.bottom);
			}
			else
			{
				SelectObject(hdc,hBrush);

				FillRect(hdc,&r,hBrush);
			}
		}
	}
}

//信息区 的绘制
VOID DrawInfo(HDC hdc)
{
	int x,y;
	int nStartX,nStartY;
	RECT rect;
	TCHAR szBuf[100];//得分的字符串

	HPEN hPen = (HPEN)GetStockObject(BLACK_PEN);
	HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);

	HBRUSH hBrush_have = (HBRUSH)GetStockObject(GRAY_BRUSH);
	SelectObject(hdc,hPen);
	SelectObject(hdc,hBrush);

	Rectangle(hdc,BOUND_SIZE*2 + GAME_X * TETRIS_SIZE,
		BOUND_SIZE,BOUND_SIZE *2 + (GAME_X + INFO_X)*TETRIS_SIZE,
		BOUND_SIZE + INFO_Y * TETRIS_SIZE);

	for (x = 0; x < 4; x++)
	{
		for (y = 0 ;y < 4 ;y++)
		{
			nStartX = BOUND_SIZE *2 + GAME_X*TETRIS_SIZE + (y +1)*TETRIS_SIZE;
			nStartY = BOUND_SIZE + (x +1)*TETRIS_SIZE;
			if (g_NextTetris[x][y])
			{
				SelectObject(hdc,g_TetrisBrush[g_NextIndex[0]]);
			}
			else
			{
				SelectObject(hdc,hBrush_have);
			}
			Rectangle(hdc,nStartX,nStartY,nStartX+TETRIS_SIZE,nStartY+TETRIS_SIZE);
		}
	}

	nStartX = BOUND_SIZE *2 + GAME_X*TETRIS_SIZE;
	nStartY = BOUND_SIZE ;

	rect.left = nStartX + TETRIS_SIZE;
	rect.right = nStartX + TETRIS_SIZE * (INFO_X -1);
	rect.top = nStartY + TETRIS_SIZE *6;
	rect.bottom = nStartY + TETRIS_SIZE *7;

	wsprintf(szBuf,L"Score: %d",g_uiScore);
	DrawText(hdc,szBuf,wcslen(szBuf),&rect,DT_CENTER);

	rect.top = nStartY + TETRIS_SIZE *7;
	rect.bottom = nStartY + TETRIS_SIZE *8;

	if (g_GameRunning)
	{
		wsprintf(szBuf,L"Running");
	}else
	{
		wsprintf(szBuf,L"Pausing");
	}	
	DrawText(hdc,szBuf,wcslen(szBuf),&rect,DT_CENTER);

	rect.top = nStartY + TETRIS_SIZE *8;
	rect.bottom = nStartY + TETRIS_SIZE *9;
	if (g_GameOver)
	{
		wsprintf(szBuf,L"GameOver");
		DrawText(hdc,szBuf,wcslen(szBuf),&rect,DT_CENTER);
	}
}

//绘制区方块，起始坐标和需要绘制的方块形状
VOID DrawTetris(HDC hdc, int nStartX,int nStartY,UINT bTetris[4][4])
{
	int y,x;
	RECT r;

	for (y = 0; y < 4; y++)
	{
		for (x = 0; x < 4; x++)
		{
			if (bTetris[y][x])
			{
				if (nStartY+y>=0&&nStartX+x>=0)
				{
					SetRect(&r,BOUND_SIZE +(nStartX + x) * TETRIS_SIZE,
						BOUND_SIZE + (nStartY + y)* TETRIS_SIZE,
						BOUND_SIZE +(nStartX + x + 1) * TETRIS_SIZE,
						BOUND_SIZE + (nStartY + y + 1)* TETRIS_SIZE);

					SelectObject(hdc,GetStockObject(BLACK_PEN));
					SelectObject(hdc,g_TetrisBrush[g_CurIndex[0]]);

					Rectangle(hdc,r.left,r.top,r.right,r.bottom);
				}

				//画落下的位置
				UINT dy = 0;
				while(CheckTetris(nStartX,nStartY+dy,bTetris,g_stGame))
				{//找到碰撞的位置
					dy++;
				}
				if (dy > 1)
				{
					dy--;
					SetRect(&r,BOUND_SIZE +(nStartX + x) * TETRIS_SIZE,
						BOUND_SIZE + (nStartY + dy + y)* TETRIS_SIZE,
						BOUND_SIZE +(nStartX + x + 1) * TETRIS_SIZE,
						BOUND_SIZE + (nStartY + dy + y + 1)* TETRIS_SIZE);

					SelectObject(hdc,g_TetrisPen[g_CurIndex[0]]);
					SelectObject(hdc,GetStockObject(NULL_BRUSH));

					Rectangle(hdc,r.left,r.top,r.right,r.bottom);
				}
			}
		}
	}
}

VOID DrawNormal(HDC hdc, LPRECT pr)
{
	SelectObject(hdcMem, GetSysColorBrush(COLOR_3DFACE));
	Rectangle(hdcMem, -1, -1, pr->right + 2, pr->bottom + 2);
	
	DrawBackGround(hdcMem);
	DrawInfo(hdcMem);
	DrawTetris(hdcMem,TetrisX,TetrixY,g_CurTetris);
}

VOID DrawAnimate(HDC hdc, int Duaring, int Part)
{
	int x,y;
	RECT r;
	HBRUSH hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);

	for (int i = 0; i < 4; i++)
	{
		if ((y = g_ToAnimateLine[i]) >= 0)
		{			
			for (x = 0; x < GAME_X; x++)
			{
				SetRect(&r,BOUND_SIZE + x*TETRIS_SIZE,
					BOUND_SIZE + y * TETRIS_SIZE,
					BOUND_SIZE + (x + 1)*TETRIS_SIZE,
					BOUND_SIZE + (y + 1) * TETRIS_SIZE);

				if (x <Part*GAME_X/Duaring)
				{
					SelectObject(hdc,hBrush);
					FillRect(hdc,&r,hBrush);
				}else
				{
					SelectObject(hdc,g_TetrisBrush[g_stGameType[y][x]]);
					SelectObject(hdc,GetStockObject(BLACK_PEN));

					Rectangle(hdc,r.left,r.top,r.right,r.bottom);
				}
			}
		}
	}
}

VOID Paint(HWND hWnd, HDC hdc, BOOL animate)
{
	RECT r;
	GetClientRect(hWnd,&r);

	hdcMem = CreateCompatibleDC(hdc);
	bmpMem = CreateCompatibleBitmap(hdc,r.right,r.bottom);
	HGDIOBJ pOld = SelectObject(hdcMem,bmpMem);

	if (animate)
	{
		int duaring = 20;
		for (int i=0; i<duaring; i++)
		{
			DrawNormal(hdcMem, &r);

			DrawAnimate(hdcMem,duaring,i);
			
			::BitBlt(hdc,0,0,r.right,r.bottom,hdcMem,0,0,SRCCOPY);

			Sleep(25);
		}
	}

	DrawNormal(hdcMem, &r);

	::BitBlt(hdc,0,0,r.right,r.bottom,hdcMem,0,0,SRCCOPY);

	SelectObject(hdcMem, pOld);
	DeleteObject(bmpMem);
	DeleteDC(hdcMem);
}

VOID Animate()
{
	HDC hdc = GetDC(g_hWnd);

	Paint(g_hWnd,hdc,TRUE);
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	int nWinX,nWinY,nClientX,nClientY;
	RECT rect;
	UINT bTmpTetris[4][4] = {};

	switch (message)
	{
	case WM_CREATE:
		//获取窗口大小
		GetWindowRect(hWnd,&rect);
		nWinX = rect.right - rect.left;
		nWinY = rect.bottom - rect.top;
		//获取客户区大小
		GetClientRect(hWnd,&rect);
		nClientX = rect.right - rect.left;
		nClientY = rect.bottom - rect.top;

		MoveWindow(hWnd,0,0,3 * BOUND_SIZE + (GAME_X + INFO_X)* TETRIS_SIZE +
			(nWinX  - nClientX),
			2 * BOUND_SIZE + GAME_Y*TETRIS_SIZE + (nWinY - nClientY),true);

		for (int i=0; i<TETRIS_CNT; ++i)
		{
			g_TetrisBrush[i] = CreateSolidBrush(g_TetrisColor[i]);
			g_TetrisPen[i] = CreatePen(PS_SOLID,2,g_TetrisColor[i]);
		}

		InitGame();
		SetTimer(hWnd,MY_TIMEER,g_uiInterval,NULL);
		g_GameRunning = TRUE;
		break;
	case WM_TIMER:
		//定时器中方块下降
		if (!g_GameRunning)
		{
			break;
		}
		if (CheckTetris(TetrisX,TetrixY + 1,g_CurTetris,g_stGame))
		{
			TetrixY++;
		}
		else
		{
			if (TetrixY <= 0)
			{
				g_GameOver = TRUE;
				g_GameRunning = FALSE;
				KillTimer(hWnd,MY_TIMEER);
				OutputDebugString(_T("WM_TIMER GameOver\r\n"));
			}else
			{
				RefreshTetris(TetrisX,TetrixY,g_CurTetris,g_stGame);
			}
			g_DownSpeed = 0;
		}
		InvalidateRect(hWnd,NULL,TRUE);
		break;
	case WM_LBUTTONDOWN:
		if (g_GameOver || !g_GameRunning)
		{
			break;
		}
		RotateTetris(g_CurTetris);
		InvalidateRect(hWnd,NULL,TRUE);
		break;
	case WM_KEYDOWN:
		if (g_GameOver)
		{
			break;
		}
		if (!g_GameRunning && wParam != VK_SPACE)
		{
			break;
		}
		switch(wParam)
		{
		case VK_SPACE:
			if (g_GameRunning)
			{
				g_GameRunning = FALSE;
			}else
			{
				g_GameRunning = TRUE;
			}
			InvalidateRect(hWnd,NULL,TRUE);
			break;
		case VK_LEFT://左方向键
			if (CheckTetris(TetrisX -1,TetrixY,g_CurTetris,g_stGame))
			{//判断一下当前方块没有靠墙就
				TetrisX--;
				InvalidateRect(hWnd,NULL,TRUE);
			}
			else
			{
				MessageBeep(0);
			}
			break;
		case  VK_RIGHT:
			if (CheckTetris(TetrisX +1,TetrixY,g_CurTetris,g_stGame))
			{//判断一下当前方块没有靠墙就
				TetrisX++;
				InvalidateRect(hWnd,NULL,TRUE);
			}
			else
			{
				MessageBeep(0);
			}
			break;
		case VK_UP://变形，但是要判断变形成功或者失败
			memcpy(bTmpTetris,g_CurTetris,sizeof(bTmpTetris));
			RotateTetris((bTmpTetris));
			if (CheckTetris(TetrisX,TetrixY,bTmpTetris,g_stGame))
			{
				//成功后，再把旋转后的copy回来
				g_CurIndex[1]=(g_CurIndex[1]+1)%g_TetrisModeCount[g_CurIndex[0]];
				memcpy(g_CurTetris,bTmpTetris,sizeof(bTmpTetris));
				InvalidateRect(hWnd,NULL,TRUE);
			}
			break;
		case VK_DOWN:
			{
				g_DownSpeed++;
				if (CheckTetris(TetrisX,TetrixY + g_DownSpeed/2,g_CurTetris,g_stGame))
				{
					TetrixY+=g_DownSpeed/2;
				}else
				{
					while (CheckTetris(TetrisX,TetrixY + 1,g_CurTetris,g_stGame))
					{
						TetrixY++;
					}
					if (TetrixY <= 0)
					{
						g_GameOver = TRUE;
						g_GameRunning = FALSE;
						KillTimer(hWnd,MY_TIMEER);
						OutputDebugString(_T("VK_DOWN GameOver\r\n"));
					}else
					{
						RefreshTetris(TetrisX,TetrixY,g_CurTetris,g_stGame);
					}
					g_DownSpeed = 0;
				}
				InvalidateRect(hWnd,NULL,TRUE);
				break;
			}

		default:
			break;
		}

		break;
	case WM_KEYUP:
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_RESTART:
			KillTimer(hWnd,MY_TIMEER);
			g_GameRunning = FALSE;

			InitGame();

			SetTimer(hWnd,MY_TIMEER,g_uiInterval,NULL);
			g_GameRunning = TRUE;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		Paint(hWnd,hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_ERASEBKGND:
		{
			return 1; //避免窗口背景的重刷
		}
	case WM_DESTROY:
		KillTimer(hWnd,MY_TIMEER);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
