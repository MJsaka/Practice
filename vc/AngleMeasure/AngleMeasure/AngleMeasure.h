
// AngleMeasure.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAngleMeasureApp:
// �йش����ʵ�֣������ AngleMeasure.cpp
//

class CAngleMeasureApp : public CWinApp
{
public:
	CAngleMeasureApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	ULONG_PTR m_gdiplusToken;
	DECLARE_MESSAGE_MAP()
};

extern CAngleMeasureApp theApp;