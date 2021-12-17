
// AngleMeasureDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AngleMeasure.h"
#include "AngleMeasureDlg.h"
#include "afxdialogex.h"

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


int const result_list_width = 360;
int const widget_gap = 5;
int const button_count = 1;
int const button_height = 30;
int const button_ids[button_count] = {
	IDC_BUTTON_LOAD,
};

int const mark_count = 10;
int const mark_len = 5;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAngleMeasureDlg 对话框
CAngleMeasureDlg::CAngleMeasureDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAngleMeasureDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_iPicWidth = 600;
	m_iPicHeight = 400;
	m_pBitmap = NULL;
	m_pBitmapDisplay = NULL;
	m_bDlgInitialized = false;
}

CAngleMeasureDlg::~CAngleMeasureDlg()
{
	if (m_pBitmap != NULL)
	{
		::delete m_pBitmap;
	}
	if (m_pBitmapDisplay != NULL)
	{
		::delete m_pBitmapDisplay;
	}
}

void CAngleMeasureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RESULT, m_listResult);
}

BEGIN_MESSAGE_MAP(CAngleMeasureDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_GETMINMAXINFO()

	ON_COMMAND(IDC_BUTTON_LOAD,OnLoadImage)
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()

void GetLineSegPointDisFrom1(double x1, double y1, double x2, double y2, double pos, double &x, double &y)
{
	double l = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));

	if (l == 0)
		return;

	double r = pos / l;

	x = x1 * (1 - r) + x2 * r;
	y = y1 * (1 - r) + y2 * r;
}

int DrawAxisOnBitmap(Gdiplus::Bitmap* bmp, int cx, int cy)
{
	Gdiplus::Graphics gr1(bmp);

	int width = bmp->GetWidth();
	int height = bmp->GetHeight();

	Gdiplus::Pen pen1(Gdiplus::Color::Red, 1);
	gr1.DrawLine(&pen1,0,cy,width,cy);
	gr1.DrawLine(&pen1,cx,0,cx,height);

	int mark_gap = min(width, height) / mark_count;

	int mark_count_x = width / mark_gap;
	int mark_count_y = height / mark_gap;

	int mark_start_x = cx - cx / mark_gap * mark_gap;
	int mark_start_y = cy - cy / mark_gap * mark_gap;

	for (int i=0; i<mark_count_x; ++i)
	{
		int x = mark_start_x + mark_gap * i;
		gr1.DrawLine(&pen1, x, cy-mark_len, x, cy+mark_len);
	}

	for (int i=0; i<mark_count_y; ++i)
	{
		int y = mark_start_y + mark_gap * i;
		gr1.DrawLine(&pen1, cx-mark_len, y, cx+mark_len, y);
	}

	return 0;
}

int DrawCircleOnBitmap(Bitmap* bmp, int x, int y, int r)
{
	Gdiplus::Graphics gr1(bmp);

	int width = bmp->GetWidth();
	int height = bmp->GetHeight();

	Gdiplus::Pen pen1(Gdiplus::Color::Red, 1);
	gr1.DrawArc(&pen1,x-r,y-r,r*2,r*2,0,360);
	
	return 0;
}

int DrawLineOnBitmap(Bitmap* bmp, int x1, int y1, int x2, int y2)
{
	Gdiplus::Graphics gr1(bmp);

	Gdiplus::Pen pen1(Gdiplus::Color::Red, 1);
	gr1.DrawLine(&pen1, x1, y1, x2, y2);

	return 0;
}

int DrawTextOnBitmap(Bitmap* bmp, CString s1, int x, int y)
{
	CStringW sw1 = s1;
	Gdiplus::Graphics gr1(bmp);

	Gdiplus::FontFamily ff1(L"新宋体");
	Gdiplus::Font f1(&ff1, 16, Gdiplus::FontStyleRegular);

	Gdiplus::StringFormat sf1;
	sf1.SetAlignment(Gdiplus::StringAlignmentCenter);
	sf1.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	Gdiplus::PointF p1(x, y);

	Gdiplus::SolidBrush sb1(Gdiplus::Color::Blue);
	gr1.DrawString(sw1, sw1.GetLength(), &f1, p1, &sf1, &sb1);

	return 0;
}

// CAngleMeasureDlg 消息处理程序
void CAngleMeasureDlg::AutoResize()
{
	RECT r;
	GetWindowRect(&r);
	r.right = r.left;
	MoveWindow(&r);
	CenterWindow();
}

BOOL CAngleMeasureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_listResult.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_ONECLICKACTIVATE|LVS_EX_LABELTIP);
	m_listResult.InsertColumn(col_point,_T("点"),LVCFMT_LEFT,60);
	m_listResult.InsertColumn(col_abspos,_T("绝对坐标"),LVCFMT_LEFT,120);
	m_listResult.InsertColumn(col_relpos,_T("相对坐标"),LVCFMT_LEFT,120);
	m_listResult.InsertColumn(col_angle,_T("角度"),LVCFMT_LEFT,60);

	m_listResult.InsertItem(row_avg,_T("平均"));
	m_listResult.InsertItem(row_center,_T("圆心"));
	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	RECT rect0;
	GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&rect0);

	m_iPicWidth = rect0.right - rect0.left;
	m_iPicHeight = rect0.bottom - rect0.top;
	// TODO: 在此添加额外的初始化代码
	m_bDlgInitialized = true;
	ClearResult();
	AutoResize();
	UpdateDisplay();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAngleMeasureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
int DrawBitmapOnWindow(HWND destWindow,Bitmap* srcBitmap)
{
	if (srcBitmap==NULL) return 0;
	if (destWindow==NULL) return 0;
	if (!IsWindow(destWindow)) return 0;
	if (!IsWindowVisible(destWindow)) return 0;

	HDC hdc1=GetDC(destWindow);

	RECT rect0;
	GetClientRect(destWindow,&rect0);

	Graphics gr1(hdc1);
	gr1.DrawImage(srcBitmap,rect0.left,rect0.top,rect0.right-rect0.left,rect0.bottom-rect0.top);
	ReleaseDC(destWindow,hdc1);

	return 0;
}

void CAngleMeasureDlg::DrawDisplay()
{
	RECT rect0;
	GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&rect0);
	int dis_width = rect0.right-rect0.left;
	int dis_height = rect0.bottom-rect0.top;

	CDC *cdc = GetDlgItem(IDC_STATIC_PIC)->GetDC();
	CDC MemDC;
	CBitmap MemBitmap;

	MemDC.CreateCompatibleDC(NULL);
	MemBitmap.CreateCompatibleBitmap(cdc, dis_width, dis_height);
	MemDC.SelectObject(&MemBitmap);
	MemDC.FillSolidRect(0, 0, dis_width, dis_height, RGB(255,255,255));

	Gdiplus::Graphics graphics(MemDC.m_hDC);
	graphics.DrawImage(m_pBitmapDisplay,0,0,dis_width,dis_height);

	cdc->BitBlt(0, 0, dis_width, dis_height, &MemDC, 0, 0, SRCCOPY);

	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
	ReleaseDC(cdc);
}

void CAngleMeasureDlg::UpdateDisplay()
{
	RECT rect0;
	GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&rect0);
	int dis_width = rect0.right-rect0.left;
	int dis_height = rect0.bottom-rect0.top;
	if (m_pBitmapDisplay!=NULL&&
		(m_pBitmapDisplay->GetWidth()!=dis_width||
		m_pBitmapDisplay->GetHeight()!=dis_height))
	{
		::delete m_pBitmapDisplay;
		m_pBitmapDisplay=NULL;
	}
	if (m_pBitmapDisplay==NULL)
	{
		m_pBitmapDisplay=::new Bitmap(dis_width,dis_height);
	}

	Gdiplus::Graphics bmpGraphics(m_pBitmapDisplay);
	bmpGraphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	if (m_pBitmap == NULL)
	{
		bmpGraphics.Clear(Gdiplus::Color::White);
	}else
	{
		bmpGraphics.DrawImage(m_pBitmap,0,0,dis_width,dis_height);
	}

	CString s1 = m_listResult.GetItemText(row_center,col_abspos);
	s1.TrimLeft(_T(" "));
	int i = s1.Find(_T(" "));
	double xc = _tstof(s1.Left(i)) * dis_width / m_iPicWidth;
	double yc = _tstof(s1.Mid(i)) * dis_height / m_iPicHeight;

	DrawAxisOnBitmap(m_pBitmapDisplay,xc,yc);


	int n = m_listResult.GetItemCount();
	if (n > row_point_start)
	{
		double radius_avg = 0;

		for (int i=row_point_start; i<n; ++i)
		{
			s1 = m_listResult.GetItemText(i,col_relpos);
			s1.TrimLeft(_T(" "));
			int j = s1.Find(_T(" "));
			double x = _tstof(s1.Left(j)) * dis_width / m_iPicWidth;
			double y = _tstof(s1.Mid(j)) * dis_height / m_iPicHeight;

			radius_avg += sqrt(x*x+y*y);


			s1 = m_listResult.GetItemText(i, col_abspos);
			s1.TrimLeft(_T(" "));
			j = s1.Find(_T(" "));
			x = _tstof(s1.Left(j)) * dis_width / m_iPicWidth;
			y = _tstof(s1.Mid(j)) * dis_height / m_iPicHeight;

			double x1, y1, x2, y2;
			GetLineSegPointDisFrom1(x, y, xc, yc, 10, x1, y1);
			GetLineSegPointDisFrom1(x, y, xc, yc, -10, x2, y2);
			DrawLineOnBitmap(m_pBitmapDisplay, x1, y1, x2, y2);

			s1.Format(_T("%d"), i - row_point_start + 1);
			DrawTextOnBitmap(m_pBitmapDisplay, s1, x, y);
		}

		radius_avg /= n - row_point_start;
		DrawCircleOnBitmap(m_pBitmapDisplay,xc,yc,radius_avg);
	}
	//DrawDisplay();
	DrawBitmapOnWindow(GetDlgItem(IDC_STATIC_PIC)->GetSafeHwnd(),m_pBitmapDisplay);
}

void CAngleMeasureDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAngleMeasureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAngleMeasureDlg::OnLoadImage()
{
	CFileDialog dlg(true,_T("*.BMP"),NULL,OFN_FILEMUSTEXIST,_T("位图文件(*.BMP)|*.BMP|所有文件(*.*)|*.*||"),this);
	if (dlg.DoModal()==IDOK)
	{
		CString FilePathName=dlg.GetPathName();
		if (m_pBitmap)
		{
			::delete m_pBitmap;
			m_pBitmap = NULL;
		}
		m_pBitmap = ::new Bitmap(FilePathName);
		CString str;
		GetWindowText(str);
		int i = str.Find(_T(" - "));
		if (i!=-1)
		{
			str = str.Left(i);
		}
		str.Append(_T(" - "));
		str.Append(FilePathName);
		SetWindowText(str);

		m_iPicWidth = m_pBitmap->GetWidth();
		m_iPicHeight = m_pBitmap->GetHeight();

		ClearResult();
		AutoResize();
	}
}

void CAngleMeasureDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if(!m_bDlgInitialized)
		return;

	RECT rect_window;
	RECT rect_client;
	RECT rect_pic;
	RECT rect_list;
	GetWindowRect(&rect_window);
	GetClientRect(&rect_client);
	GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&rect_pic);
	GetDlgItem(IDC_LIST_RESULT)->GetClientRect(&rect_list);

	int client_w = rect_client.right - rect_client.left;
	int client_h = rect_client.bottom - rect_client.top;

	int pic_h = client_h - widget_gap * 2;
	int pic_w = pic_h * m_iPicWidth / m_iPicHeight;

	int client_new_w = pic_w + result_list_width + widget_gap * 3;

	int w_diff = client_new_w - client_w;
	if (w_diff!=0)
	{
		rect_window.right += w_diff;
		MoveWindow(&rect_window);
		CenterWindow();
		return;
	}

	rect_pic.left = widget_gap;
	rect_pic.top = widget_gap;
	rect_pic.right = rect_pic.left + pic_w;
	rect_pic.bottom = rect_pic.top + pic_h;
	GetDlgItem(IDC_STATIC_PIC)->MoveWindow(&rect_pic);

	rect_list.left = pic_w + widget_gap * 2;
	rect_list.top = widget_gap;
	rect_list.right = rect_list.left + result_list_width;
	rect_list.bottom = client_h - widget_gap * 2 - (button_height+widget_gap) * button_count;
	GetDlgItem(IDC_LIST_RESULT)->MoveWindow(&rect_list);
	
	RECT r;
	r.left = pic_w + widget_gap * 2;
	r.right = r.left + result_list_width;
	r.top = rect_list.bottom + widget_gap;
	r.bottom = r.top + button_height;
	for (int i=0; i < button_count; ++i)
	{
		GetDlgItem(button_ids[i])->MoveWindow(&r);
		r.top = r.bottom + widget_gap;
		r.bottom = r.top + button_height;
	}
}

void CAngleMeasureDlg::CalAvg()
{
	CString s1;
	int n = m_listResult.GetItemCount();
	if (n > row_point_start)
	{
		double avg = 0;
		for (int i = row_point_start; i < n; ++i)
		{
			s1 = m_listResult.GetItemText(i,col_angle);
			avg += _tstof(s1);
		}
		avg /= (n-row_point_start);
		s1.Format(_T("%6.2f"),avg);
		m_listResult.SetItemText(row_avg,col_angle,s1);
	}else
	{
		m_listResult.SetItemText(row_avg,col_angle,_T(""));
	}
	m_listResult.SetItemState(row_avg, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

inline bool IsDbClick()
{
	//屏蔽双击时的单击事件
	DWORD st = GetTickCount();
	MSG message;
	while(true)
	{
		if(::PeekMessage(&message, NULL, 0 ,0 , PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
			if(message.message == WM_LBUTTONDBLCLK)
			{	//判断为双击
				return true;
			}
		}

		DWORD et = GetTickCount();
		if(et - st > 300)
		{	//判断为单击
			return false;
		}
	}
}

void CAngleMeasureDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CWnd::OnLButtonDown(nFlags, point);
	if (IsDbClick())
	{
		return;
	}

	RECT r;
	GetDlgItem(IDC_STATIC_PIC)->GetWindowRect(&r);
	ScreenToClient(&r);
	
	if (point.x < r.left || point.x > r.right ||
		point.y < r.top || point.y > r.bottom)
	{
		return;
	}
	//转换到图像像素坐标
	double x = double(point.x - r.left) * m_iPicWidth / (r.right - r.left);
	double y = double(point.y - r.top) * m_iPicHeight / (r.bottom - r.top);

	CString s1;
	int n = m_listResult.GetItemCount();
	s1.Format(_T("点%d"),n-row_point_start+1);
	m_listResult.InsertItem(n,s1);
	s1.Format(_T("%8.2f  %8.2f"),x,y);
	m_listResult.SetItemText(n,col_abspos,s1);

	//计算相对原点坐标
	s1 = m_listResult.GetItemText(row_center,col_abspos);
	s1.TrimLeft(_T(" "));
	int i = s1.Find(_T(" "));
	double xc = _tstof(s1.Left(i));
	double yc = _tstof(s1.Mid(i));
	x = x - xc;
	y = yc - y;

	s1.Format(_T("%8.2f  %8.2f"),x,y);
	m_listResult.SetItemText(n,col_relpos,s1);

	double angle = acos(y/sqrt(x*x + y*y));
	if (x<0)
	{
		angle = M_PI * 2 - angle;
	}

	angle = angle * M_1_PI * 180;
	s1.Format(_T("%6.2f"),angle);
	m_listResult.SetItemText(n,col_angle,s1);

	CalAvg();
	GetDlgItem(IDC_STATIC_PIC)->Invalidate(TRUE);
	GetDlgItem(IDC_STATIC_PIC)->UpdateWindow();
}


void CAngleMeasureDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnLButtonDblClk(nFlags, point);

	RECT r;
	GetDlgItem(IDC_STATIC_PIC)->GetWindowRect(&r);
	ScreenToClient(&r);

	if (point.x < r.left || point.x > r.right ||
		point.y < r.top || point.y > r.bottom)
	{
		return;
	}

	ClearResult();

	//转换到图像像素坐标
	double x = double(point.x - r.left) * m_iPicWidth / (r.right - r.left);
	double y = double(point.y - r.top) * m_iPicHeight / (r.bottom - r.top);

	CString s1;
	s1.Format(_T("%8.2f  %8.2f"),x,y);
	m_listResult.SetItemText(row_center,col_abspos,s1);
	s1.Format(_T("%8.2f  %8.2f"),0.0,0.0);
	m_listResult.SetItemText(row_center,col_relpos,s1);

	GetDlgItem(IDC_STATIC_PIC)->Invalidate(TRUE);
	GetDlgItem(IDC_STATIC_PIC)->UpdateWindow();
}


void CAngleMeasureDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnRButtonDown(nFlags, point);

	RECT r;
	GetDlgItem(IDC_STATIC_PIC)->GetWindowRect(&r);
	ScreenToClient(&r);

	if (point.x < r.left || point.x > r.right ||
		point.y < r.top || point.y > r.bottom)
	{
		return;
	}

	int n = m_listResult.GetItemCount();
	if (n>row_point_start)
	{
		m_listResult.DeleteItem(n-1);
	}
	CalAvg();

	GetDlgItem(IDC_STATIC_PIC)->Invalidate(TRUE);
	GetDlgItem(IDC_STATIC_PIC)->UpdateWindow();
}


void CAngleMeasureDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize.y = 400;
	lpMMI->ptMaxTrackSize.y = 800;
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


void CAngleMeasureDlg::ClearResult()
{
	// TODO: 在此添加控件通知处理程序代码
	int n = m_listResult.GetItemCount();
	while (n>row_point_start)
	{
		m_listResult.DeleteItem(n-1);
		n--;
	}
	m_listResult.SetItemText(row_avg,col_angle,_T(""));

	CString s1;
	s1.Format(_T("%8.2f  %8.2f"),m_iPicWidth*0.5,m_iPicHeight*0.5);
	m_listResult.SetItemText(row_center,col_abspos,s1);
	s1.Format(_T("%8.2f  %8.2f"),0.0,0.0);
	m_listResult.SetItemText(row_center,col_relpos,s1);
}


void CAngleMeasureDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDCtl==IDC_STATIC_PIC)
	{
		UpdateDisplay();
	}
	CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
