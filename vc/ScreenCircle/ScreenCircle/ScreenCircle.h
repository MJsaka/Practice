
// ScreenCircle.h : ScreenCircle Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CScreenCircleApp:
// �йش����ʵ�֣������ ScreenCircle.cpp
//

class CScreenCircleApp : public CWinApp
{
public:
	CScreenCircleApp();

	ULONG_PTR m_gdiplusToken;
// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

public:
	DECLARE_MESSAGE_MAP()
};

extern CScreenCircleApp theApp;
