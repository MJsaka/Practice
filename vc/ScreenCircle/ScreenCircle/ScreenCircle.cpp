
// ScreenCircle.cpp : ����Ӧ�ó��������Ϊ��
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


// CScreenCircleApp ����

CScreenCircleApp::CScreenCircleApp()
{
	// TODO: ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("com.mjsaka.ScreenCircles"));

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CScreenCircleApp ����

CScreenCircleApp theApp;


// CScreenCircleApp ��ʼ��

BOOL CScreenCircleApp::InitInstance()
{
	CWinApp::InitInstance();

#if FLOAT_WINDOW_INSTANCE
	EnableTaskbarInteraction(FALSE);
#endif

	//����Gdiplus
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	SetRegistryKey(_T("com.mjsaka.ScreenCircles"));


	// ��Ҫ���������ڣ��˴��뽫�����µĿ�ܴ���
	// ����Ȼ��������ΪӦ�ó���������ڶ���
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	
	CREATESTRUCT cs = {0};

#if FLOAT_WINDOW_INSTANCE
	cs.cx = GetSystemMetrics(SM_CXSCREEN);
	cs.cy = GetSystemMetrics(SM_CYSCREEN);

	//���ô�����ʽ  �ֲ���  �ö�  ������  ͸��
	cs.dwExStyle |= WS_EX_LAYERED;
	cs.dwExStyle |= WS_EX_TOPMOST;
	cs.dwExStyle |= WS_EX_NOACTIVATE;
	cs.dwExStyle |= WS_EX_TRANSPARENT;
	cs.lpszClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW);
	pFrame->CreateEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style, cs.x, cs.y, cs.cx, cs.cy, NULL, NULL);

	//ȥ�����ڱ߿�
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

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	return TRUE;
}

int CScreenCircleApp::ExitInstance()
{
	//TODO: �����������ӵĸ�����Դ
	//�ر�Gdiplus
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	return CWinApp::ExitInstance();
}



