
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "ScreenCircle.h"
#include "CalCircle.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define   WM_SYSTEMTRAY WM_USER+1

#include <gdiplus.h>

Gdiplus::Bitmap * g_pBitmap1;
//Բ����ɫ
DWORD circle_colors[MAX_COUNT];

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
DWORD get_color(float x) {
	float r = 0.0f;
	float g = 0.0f;
	float b = 1.0f;
	if (x >= 0.0f && x < 0.2f) {
		x = x / 0.2f;
		r = 0.0f;
		g = x;
		b = 1.0f;
	}
	else if (x >= 0.2f && x < 0.4f) {
		x = (x - 0.2f) / 0.2f;
		r = 0.0f;
		g = 1.0f;
		b = 1.0f - x;
	}
	else if (x >= 0.4f && x < 0.6f) {
		x = (x - 0.4f) / 0.2f;
		r = x;
		g = 1.0f;
		b = 0.0f;
	}
	else if (x >= 0.6f && x < 0.8f) {
		x = (x - 0.6f) / 0.2f;
		r = 1.0f;
		g = 1.0f - x;
		b = 0.0f;
	}
	else if (x >= 0.8f && x <= 1.0f) {
		x = (x - 0.8f) / 0.2f;
		r = 1.0f;
		g = 0.0f;
		b = x;
	}
	return RGB(r * 255, g * 255, b * 255) | 0xFF000000;
}
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CWnd)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_NCACTIVATE()
	ON_WM_NCCALCSIZE()
	ON_MESSAGE(WM_SYSTEMTRAY, &CMainFrame::OnSystemtray)
	ON_COMMAND(ID_TRAY_EXIT, &CMainFrame::OnTrayExit)
	ON_COMMAND(ID_TRAY_PAUSE, &CMainFrame::OnTrayPause)
	ON_COMMAND(ID_TRAY_STEP, &CMainFrame::OnTrayStep)
END_MESSAGE_MAP()

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
	g_pBitmap1 = NULL;
}

CMainFrame::~CMainFrame()
{
}


// CMainFrame ���

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
//����Բ��λͼ
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

#if 0
	Pen pen(Color::Black);

	for (int i = 1; i < m_row; i++)
	{
		gr1.DrawLine(&pen, 0, i*m_height, border_width, i*m_height);
	}
	for (int i = 1; i < m_col; i++)
	{
		gr1.DrawLine(&pen, i*m_width, 0, i*m_width, border_height);
	}
#endif

	for (int i = 0; i < CIRCLE_COUNT; i++)
	{
		int x = int(pos_x[i] - circle_radius);
		int y = int(pos_y[i] - circle_radius);
		int w = int(circle_radius * 2);
		int h = int(circle_radius * 2);
		Rect rect(x, y, w, h);

		if (1)
		{//����ʵ��Բ
			SolidBrush brush1(circle_colors[i]);
			gr1.FillEllipse(&brush1, rect);
		}
		else
		{//����ͼƬ
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
		{//�������ֱ��	
			FontFamily fontfamily1(L"������");
			Gdiplus::Font font1(&fontfamily1, circle_radius / 2, FontStyleBold);

			StringFormat stringformatcentercenter;//����
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

	m_row = border_height / (2.2 * circle_radius);
	m_col = border_width / (2.2 * circle_radius);
	//����circle_radius���������������е�Բ
	while (m_row * m_col < CIRCLE_COUNT)
	{
		circle_radius-=8;
		m_row = border_height / (2.2 * circle_radius);
		m_col = border_width / (2.2 * circle_radius);
	}
	//���������������٣�ʹÿ�����ӿ�����
	while (m_row * m_col > CIRCLE_COUNT * 2)
	{
		if (m_row > m_col)
			m_row--;
		else
			m_col--;
	}
	//ÿ�����ӵĿ��
	m_width = border_width / m_col;
	m_height = border_height / m_row;
	//��ʼ��Բ��λ��
	int k = 0;
	for (int i = 0; i < m_row && k < CIRCLE_COUNT; i++)
	{
		for (int j = 0; j < m_col && k < CIRCLE_COUNT; j++, k++)
		{
			pos_x[k] = (j + 0.5) * m_width;
			pos_y[k] = (i + 0.5) * m_height;
		}
	}
	//��ʼ��Բ���ٶ�
	for (int i = 0; i < CIRCLE_COUNT; i++)
	{
		spd_x[i] = (rand() % 2 - 2) * i % 4 * base_speed;
		spd_y[i] = (rand() % 2 - 2) * i % 4 * base_speed;
	}
}
void CMainFrame::InitColors()
{
	for (int i = 0; i < MAX_COUNT; i++)
	{
		circle_colors[i] = get_color(i * 1.0 / MAX_COUNT);
	}
}

UINT __cdecl CircleThreadFunc(LPVOID param)
{
	CMainFrame * p = (CMainFrame *)param;

	long k = 0;

	p->m_running = true;

	while (p->m_torun)
	{
		if (!p->m_pause)
		{
			//����λ��
			CalCircleCycle();

			//����Բ��λͼ
			p->m_cs.Lock(INFINITE);
			p->DrawCirclesToBmp();
			p->m_cs.Unlock();

			p->Invalidate(TRUE);
			
			k++;

			if (k == 0)
			{
				p->m_step = true;
			}
			
			if (p->m_step)
			{
				p->m_pause = true;
			}
		}

		Sleep(50);
	}

	p->m_running = false;

	return 0;
}

void CMainFrame::PostNcDestroy()
{
	// TODO:  �ڴ����ר�ô����/����û���
	delete this;
	CWnd::PostNcDestroy();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

	//����ϵͳ����
	NotifyIcon.cbSize = sizeof(NOTIFYICONDATA);
	NotifyIcon.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	NotifyIcon.hWnd = m_hWnd;
	lstrcpy(NotifyIcon.szTip, _T("Bubble"));
	NotifyIcon.uCallbackMessage = WM_SYSTEMTRAY;
	NotifyIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	Shell_NotifyIcon(NIM_ADD, &NotifyIcon);   //���ϵͳ����

	m_torun = true;
	m_pause = false;
	m_step = false;

	InitColors();
	InitCircles();
	AfxBeginThread(CircleThreadFunc, this);

	return 0;
}


void CMainFrame::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO:  �ڴ˴������Ϣ����������
	Shell_NotifyIcon(NIM_DELETE, &NotifyIcon);//��������ͼ��

	m_torun = false;

	//�ȴ��߳��˳�
	while (m_running)
	{
		Sleep(10);
	}

	if (g_pBitmap1)
	{
		::delete g_pBitmap1;
		g_pBitmap1 = NULL;
	}
}

void CMainFrame::OnPaint()
{
	//���ô��ڻ������,����ɫ��Ϊ͸��
	SetLayeredWindowAttributes(RGB(255,255,255), 0, LWA_COLORKEY);

	CPaintDC dc(this); // ���ڻ��Ƶ��豸������
	//����λͼ������
	Gdiplus::Graphics gr1(dc.m_hDC);

	m_cs.Lock(INFINITE);
	gr1.DrawImage(g_pBitmap1, 0, 0, border_width, border_height);
	m_cs.Unlock();
	// ��ҪΪ������Ϣ������ CWnd::OnPaint()
}

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	//˫������� ��ֹ��������
	return TRUE;

	return CWnd::OnEraseBkgnd(pDC);
}

BOOL CMainFrame::OnNcActivate(BOOL bActive)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return TRUE;

	return CWnd::OnNcActivate(bActive);
}

void CMainFrame::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	//�ޱ߿򴰿�
	return;

	return CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
}

afx_msg LRESULT CMainFrame::OnSystemtray(WPARAM wParam, LPARAM lParam)
{
	//wParam���յ���ͼ���ID����lParam���յ���������Ϊ
	     
	switch (lParam)
	{
	case  WM_LBUTTONDOWN://��������Ĵ���     
	{
		if (IsWindowVisible())
		{
			ShowWindow(SW_HIDE);
		}
		else
		{
			ShowWindow(SW_SHOW);
		}

		break;
	}
	case  WM_RBUTTONDOWN://�Ҽ�����ʱ������ݲ˵�
	{
		CMenu traymenu;
		traymenu.LoadMenu(IDR_MENU_TRAY);
		CMenu *pPopup = traymenu.GetSubMenu(0);

		pPopup->CheckMenuItem(ID_TRAY_PAUSE, m_pause ? MF_CHECKED : MF_UNCHECKED);
		pPopup->CheckMenuItem(ID_TRAY_STEP, m_step ? MF_CHECKED : MF_UNCHECKED);

		CPoint mypoint;
		GetCursorPos(&mypoint);
		//ClientToScreen(&mypoint);//���ͻ�������ת��Ϊ��Ļ����
		SetForegroundWindow();
		PostMessage(WM_NULL, 0, 0);

		//��ʾ�Ҽ��˵��������ര��ӵ�С�
		pPopup->TrackPopupMenu(TPM_LEFTALIGN, mypoint.x, mypoint.y, this);

		break;
	}
	}
	return 0;
}

void CMainFrame::OnTrayPause()
{
	// TODO:  �ڴ���������������
	m_pause = !m_pause;
}


void CMainFrame::OnTrayStep()
{
	// TODO:  �ڴ���������������
	m_step = !m_step;
}

void CMainFrame::OnTrayExit()
{
	// TODO:  �ڴ���������������
	DestroyWindow();
}

