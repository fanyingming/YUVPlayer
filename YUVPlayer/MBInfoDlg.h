#if !defined(AFX_MBINFODLG_H__06E4523D_E149_4B00_A40B_08907B00880A__INCLUDED_)
#define AFX_MBINFODLG_H__06E4523D_E149_4B00_A40B_08907B00880A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MBInfoDlg.h : header file
//

#include "define.h"
#include "YUVPlayer.h"

/////////////////////////////////////////////////////////////////////////////
// CMBInfoDlg dialog

class CMBInfoDlg : public CDialog
{
// Construction
public:
	CMBInfoDlg(CWnd* pParent = NULL);   // standard constructor
    
	uint8	u8LumaPointNumX;		//++ 亮度水平方向需要显示的点数
	uint8	u8LumaPointNumY;		//++ 亮度垂直方向需要显示的点数
	uint8	u8ChroPointNumX;		//++ 色度水平方向需要显示的点数
	uint8	u8ChroPointNumY;		//++ 色度垂直方向需要显示的点数
	uint8	pixelValue[26][17];		//++ 宏块像素值
	int32	s32GridWidth;			//++ 单元格宽度
	int32	s32GridHeight;			//++ 单元格高度
	int32	s32PixTableX;			//++ 表格顶点坐标
	int32	s32PixTableY;			//++ 表格顶点坐标
	CString	gridValue[26][17];		//++ 宏块像素值
	CRect	gridRect[26][17];		//++ 单元格
	CFont	talbeFont;
	CRect	oldRect;			//++ 被选定像素点标记区
	CRect	newRect;			//++ 被选定像素点标记区
	CWnd	*pParentWnd;
	
	void	create_window(CWnd *pWnd);
	void	draw_pixel_table();
	void	clean_mark();

// Dialog Data
	//{{AFX_DATA(CMBInfoDlg)
	enum { IDD = IDD_MBINFO_DIALOG };
	CString	m_sMBInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMBInfoDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMBInfoDlg)
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	uint8	u8ShowMark;			//++ 是否显示选定像素点标记
	uint8	u8ValueMode;		//++ 像素值是否以十六进制方式显示
	int32	s32MarkGridX;		//++ 被选定像素点的坐标
	int32	s32MarkGridY;		//++ 被选定像素点的坐标
	
	void	initial();
	void	get_grid_value();
	void	draw_pixel_value();
	void	draw_mark(CRect &currRect);
	void	mark_selected_pixel();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MBINFODLG_H__06E4523D_E149_4B00_A40B_08907B00880A__INCLUDED_)
