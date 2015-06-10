#if !defined(AFX_CUSTOMSIZEDLG_H__B929BB92_BEFF_4DDD_ABA4_B588DFA95FD3__INCLUDED_)
#define AFX_CUSTOMSIZEDLG_H__B929BB92_BEFF_4DDD_ABA4_B588DFA95FD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomSizeDlg.h : header file
//

#include "define.h"
#include "YUVPlayer.h"

/////////////////////////////////////////////////////////////////////////////
// CCustomSizeDlg dialog

class CCustomSizeDlg : public CDialog
{
// Construction
public:
	CCustomSizeDlg(CWnd* pParent = NULL);   // standard constructor
	CCustomSizeDlg(CWnd* pParent, CWnd* pMainDlg);

    CWnd	*pMainDlg;
	
	void	get_size_list();
	void	create_window(CWnd *pParentWnd);
	void	close_dlg();
// Dialog Data
	//{{AFX_DATA(CCustomSizeDlg)
	enum { IDD = IDD_CUSTOMSIZE_DIALOG };
	CEdit	m_width;
	CListCtrl	m_sizeList;
	CString	m_sHeight;
	CString	m_sWidth;
	CString	m_sPriority;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomSizeDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCustomSizeDlg)
	virtual void OnOK();
	afx_msg void OnDeleteItems();
	virtual void OnCancel();
	afx_msg void OnNoSelect();
	afx_msg void OnClose();
	afx_msg void OnClickSizeList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClearSize();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString		frameSize;

	void	initial();
	void	add_item();
	void	reset_input();
	void	set_frame_size(int8 s8Offset);
    void	update_framesize_list();
	int32	check_size();
	int32	get_frame_size();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMSIZEDLG_H__B929BB92_BEFF_4DDD_ABA4_B588DFA95FD3__INCLUDED_)
