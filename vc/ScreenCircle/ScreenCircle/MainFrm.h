
// MainFrm.h : CMainFrame 类的接口
//

#pragma once

class CMainFrame : public CWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	volatile bool m_torun;
	volatile bool m_running;
	volatile bool m_pause;
	volatile bool m_step;

	int m_row, m_col;
	int m_width, m_height;

	CCriticalSection m_cs;

	NOTIFYICONDATA NotifyIcon;  //系统托盘类
// 生成的消息映射函数
public:
	void InitCircles();
	void InitColors();
	void DrawCirclesToBmp();

	DECLARE_MESSAGE_MAP()

	virtual void PostNcDestroy();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);

	afx_msg LRESULT OnSystemtray(WPARAM wParam, LPARAM lParam);

	afx_msg void OnTrayExit();
	afx_msg void OnTrayPause();
	afx_msg void OnTrayStep();
};


