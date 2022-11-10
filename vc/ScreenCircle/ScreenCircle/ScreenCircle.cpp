
// ScreenCircle.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "ScreenCircle.h"
#include "MainFrm.h"

#include <gdiplus.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CScreenCircleApp

BEGIN_MESSAGE_MAP(CScreenCircleApp, CWinApp)
END_MESSAGE_MAP()


// CScreenCircleApp 构造

CScreenCircleApp::CScreenCircleApp()
{
	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("com.mjsaka.ScreenCircles"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CScreenCircleApp 对象

CScreenCircleApp theApp;


// CScreenCircleApp 初始化

BOOL CScreenCircleApp::InitInstance()
{
	CWinApp::InitInstance();

#if FLOAT_WINDOW_INSTANCE
	EnableTaskbarInteraction(FALSE);
#endif

	//启动Gdiplus
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	SetRegistryKey(_T("com.mjsaka.ScreenCircles"));


	// 若要创建主窗口，此代码将创建新的框架窗口
	// 对象，然后将其设置为应用程序的主窗口对象
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	
	CREATESTRUCT cs = {0};

#if FLOAT_WINDOW_INSTANCE
	cs.cx = GetSystemMetrics(SM_CXSCREEN);
	cs.cy = GetSystemMetrics(SM_CYSCREEN);

	//设置窗口样式  分层混合  置顶  不激活  透明
	cs.dwExStyle |= WS_EX_LAYERED;
	cs.dwExStyle |= WS_EX_TOPMOST;
	cs.dwExStyle |= WS_EX_NOACTIVATE;
	cs.dwExStyle |= WS_EX_TRANSPARENT;
	cs.lpszClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW);
	pFrame->CreateEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style, cs.x, cs.y, cs.cx, cs.cy, NULL, NULL);

	//去除窗口边框
	::SetWindowLong(pFrame->m_hWnd, GWL_STYLE, ::GetWindowLong(pFrame->m_hWnd, GWL_STYLE) & ~WS_BORDER);
	::SetWindowLong(pFrame->m_hWnd, GWL_STYLE, ::GetWindowLong(pFrame->m_hWnd, GWL_STYLE) & ~WS_CAPTION);
	::SetWindowLong(pFrame->m_hWnd, GWL_STYLE, ::GetWindowLong(pFrame->m_hWnd, GWL_STYLE) & ~WS_EX_WINDOWEDGE);
	::SetWindowLong(pFrame->m_hWnd, GWL_STYLE, ::GetWindowLong(pFrame->m_hWnd, GWL_STYLE) & ~WS_EX_DLGMODALFRAME);
#else
	cs.cx = 800;
	cs.cy = 600;
	cs.x = (GetSystemMetrics(SM_CXSCREEN) - cs.cx) / 2;
	cs.y = (GetSystemMetrics(SM_CYSCREEN) - cs.cy) / 2;
	cs.dwExStyle = WS_EX_OVERLAPPEDWINDOW;
	cs.lpszClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW);
	pFrame->CreateEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style, cs.x, cs.y, cs.cx, cs.cy, GetDesktopWindow(), NULL);
#endif

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	return TRUE;
}

int CScreenCircleApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	//关闭Gdiplus
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	return CWinApp::ExitInstance();
}



