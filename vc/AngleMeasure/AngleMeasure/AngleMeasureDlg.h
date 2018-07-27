
// AngleMeasureDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "resource.h"

#include <Gdiplus.h>
using namespace Gdiplus;
// CAngleMeasureDlg 对话框
class CAngleMeasureDlg : public CDialogEx
{
// 构造
public:
	CAngleMeasureDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CAngleMeasureDlg();

// 对话框数据
	enum { IDD = IDD_ANGLEMEASURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	Bitmap *m_pBitmap;
	Bitmap *m_pBitmapDisplay;

	int m_iPicWidth;
	int m_iPicHeight;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

public:
	bool m_bDlgInitialized;
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnLoadImage();
	void UpdateDisplay();
	void DrawDisplay();
	void AutoResize();

	void CalAvg();
	void ClearResult();
	DECLARE_MESSAGE_MAP()
public:

	enum {col_point,col_abspos,col_relpos,col_angle};
	enum {row_avg,row_center,row_point_start};
	CListCtrl m_listResult;
};
