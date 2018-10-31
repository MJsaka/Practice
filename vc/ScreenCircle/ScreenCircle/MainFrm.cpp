
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "ScreenCircle.h"
#include "CalCircle.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include <gdiplus.h>

Gdiplus::Bitmap * g_pBitmap1;
//圆的颜色
DWORD circle_colors[MAX_COUNT] =
{
	0xFF3F3F3F,
	0xFF6F6F6F,
	0xFF9F9F9F,
	0xFFCFCFCF,

	0xFF7F0000,
	0xFF007F00,
	0xFF00007F,

	0xFFFF0000,
	0xFF00FF00,
	0xFF0000FF,

	0xFF7F7F00,
	0xFF007F7F,
	0xFF7F007F,

	0xFFFFFF00,
	0xFFFF00FF,
	0xFF00FFFF,
};

struct stARGB
{
	union 
	{
		DWORD argb;
		struct
		{
			unsigned char b;
			unsigned char g;
			unsigned char r;
			unsigned char a;
		};
	};
	
	stARGB(DWORD d) :argb(d) {};
};

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CWnd)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_NCACTIVATE()
	ON_WM_NCCALCSIZE()
END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	g_pBitmap1 = NULL;
}

CMainFrame::~CMainFrame()
{
	if (g_pBitmap1)
	{
		::delete g_pBitmap1;
		g_pBitmap1 = NULL;
	}
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);
}
#endif //_DEBUG
//绘制圆到位图
void CMainFrame::DrawCirclesToBmp()
{
	using namespace Gdiplus;
	if (g_pBitmap1 != NULL)
	{
		int w = g_pBitmap1->GetWidth();
		int h = g_pBitmap1->GetHeight();
		if (w != border_width || h != border_height)
		{
			::delete g_pBitmap1;
			g_pBitmap1 = NULL;
		}
	}
	if (g_pBitmap1 == NULL)
	{
		g_pBitmap1 = ::new Bitmap(border_width, border_height);
	}

	Graphics gr1(g_pBitmap1);
	gr1.SetSmoothingMode(SmoothingModeAntiAlias);
	gr1.Clear(Color::White);

	for (int i = 0; i < CIRCLE_COUNT; i++)
	{
		int x = int(pos_x[i] - circle_radius);
		int y = int(pos_y[i] - circle_radius);
		int w = int(circle_radius * 2);
		int h = int(circle_radius * 2);
		Rect rect(x, y, w, h);

		if (1)
		{//绘制实心圆
			SolidBrush brush1(circle_colors[i]);
			gr1.FillEllipse(&brush1, rect);
		}
		else
		{//绘制图片
			struct stARGB color1(circle_colors[i]);
			Gdiplus::ColorMatrix mat = { 0 };
			mat.m[0][0] = float(color1.r) / 128;
			mat.m[1][1] = float(color1.g) / 128;
			mat.m[2][2] = float(color1.b) / 128;
			mat.m[3][3] = 1.0;

			Image img(_T("circle.png"));
			Gdiplus::ImageAttributes attr;
			attr.SetColorMatrix(&mat);
			gr1.DrawImage(&img, rect, 0, 0, img.GetWidth(), img.GetHeight(), Gdiplus::UnitPixel, &attr);
		}

		if (1)
		{//绘制文字编号	
			FontFamily fontfamily1(L"新宋体");
			Gdiplus::Font font1(&fontfamily1, circle_radius / 2, FontStyleBold);

			StringFormat stringformatcentercenter;//中中
			stringformatcentercenter.SetAlignment(StringAlignmentCenter);
			stringformatcentercenter.SetLineAlignment(StringAlignmentCenter);

			SolidBrush brush1(Color::Black);

			RectF rectf(x, y, w, h);

			CStringW sw1;
			sw1.Format(_T("%d"), i);
			gr1.DrawString(sw1, sw1.GetLength(), &font1, rectf, &stringformatcentercenter, &brush1);
		}
	}
}

void CMainFrame::InitCircles()
{
	CRect rect;
	GetWindowRect(&rect);
	border_width = rect.right - rect.left;
	border_height = rect.bottom - rect.top;

	int row, col;
	row = border_height / (2.2 * circle_radius);
	col = border_width / (2.2 * circle_radius);
	//调整circle_radius至窗口能容纳所有的圆
	while (row * col < CIRCLE_COUNT)
	{
		circle_radius-=8;
		row = border_height / (2.2 * circle_radius);
		col = border_width / (2.2 * circle_radius);
	}
	//调整行列数至最少，使每个格子宽高最大
	while (row * col > CIRCLE_COUNT * 2)
	{
		if (row > col)
			row--;
		else
			col--;
	}
	//每个格子的宽高
	int width = border_width / col;
	int height = border_height / row;
	//初始化圆的位置
	int k = 0;
	for (int i = 0; i < row && k < CIRCLE_COUNT; i++)
	{
		for (int j = 0; j < col && k < CIRCLE_COUNT; j++, k++)
		{
			pos_x[k] = (j + 0.5) * width;
			pos_y[k] = (i + 0.5) * height;
		}
	}
	//初始化圆的速度
	for (int i = 0; i < CIRCLE_COUNT; i++)
	{
		spd_x[i] = (rand() % 5 - 2) * base_speed;
		spd_y[i] = (rand() % 5 - 2) * base_speed;
	}
}

UINT __cdecl CircleThreadFunc(LPVOID param)
{
	CMainFrame * p = (CMainFrame *)param;
	while (true)
	{
		CalCircleCycle();
		p->Invalidate(TRUE);
		Sleep(16);
	}
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	InitCircles();
	AfxBeginThread(CircleThreadFunc, this);
}

void CMainFrame::OnPaint()
{
	//设置窗口混合属性,将白色变为透明
	SetLayeredWindowAttributes(RGB(255,255,255), 0, LWA_COLORKEY);
	//绘制圆到位图
	DrawCirclesToBmp();

	CPaintDC dc(this); // 用于绘制的设备上下文
	//绘制位图到界面
	Gdiplus::Graphics gr1(dc.m_hDC);
	gr1.DrawImage(g_pBitmap1, 0, 0, border_width, border_height);
	// 不要为绘制消息而调用 CWnd::OnPaint()
}

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//双缓冲绘制 禁止背景擦除
	return TRUE;

	return CWnd::OnEraseBkgnd(pDC);
}

BOOL CMainFrame::OnNcActivate(BOOL bActive)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;

	return CWnd::OnNcActivate(bActive);
}

void CMainFrame::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	//无边框窗口
	return;

	return CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
}
