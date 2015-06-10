#if !defined(AFX_DIFFPICDLG_H__EE636417_B4C3_409F_A192_3DAC0039CF24__INCLUDED_)
#define AFX_DIFFPICDLG_H__EE636417_B4C3_409F_A192_3DAC0039CF24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DiffPicDlg.h : header file
//

#include "define.h"
#include "ChildDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CDiffPicDlg dialog

class CDiffPicDlg : public CChildDlg
{
// Construction
public:
    CDiffPicDlg(CWnd* pParent = NULL);   // standard constructor

	uint8	bSameFlag;
	CFont	font;

    void	button_down_left();
    void	button_down_right();
    LRESULT	show_one_frame(WPARAM wParam, LPARAM lParam);
    void	get_one_frame(uint8 u8ImageMode);
    void	show_window();
	void	kill_window();
    int32	create_window(CWnd *pParentWnd);

// Dialog Data
	//{{AFX_DATA(CDiffPicDlg)
	enum { IDD = IDD_DIFFPIC_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiffPicDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDiffPicDlg)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMenuitemJump();
	afx_msg void OnMenuitemYUV();
	afx_msg void OnMenuitemY();
	afx_msg void OnMenuitemU();
	afx_msg void OnMenuitemV();
	afx_msg void OnMenuitemBinMode();
	afx_msg void OnMenuitemAbsMode();
	afx_msg void OnMenuitemShowmbinfo();
	afx_msg void OnMenuitemNotice();
	afx_msg void OnMenuitemAttach();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg LRESULT OnEnterSizeMove(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnExitSizeMove(WPARAM wp, LPARAM lp);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
        
private:
    uint8	u8DiffMode;				//++ 差值计算方式
    
    int32	initial();
    CString	get_title();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIFFPICDLG_H__EE636417_B4C3_409F_A192_3DAC0039CF24__INCLUDED_)
