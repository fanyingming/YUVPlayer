#if !defined(AFX_JUMPFRAMEDLG_H__8062FC12_6A27_42DA_9052_F0D056107FC5__INCLUDED_)
#define AFX_JUMPFRAMEDLG_H__8062FC12_6A27_42DA_9052_F0D056107FC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JumpFrameDlg.h : header file
//

#include "define.h"
#include "YUVPlayer.h"
#include "ImageDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CJumpFrameDlg dialog

class CJumpFrameDlg : public CDialog
{
// Construction
public:
	CJumpFrameDlg(CWnd* pParent = NULL);   // standard constructor
	CJumpFrameDlg(CWnd* pParent, CWnd* pMainDlg);

	uint8	bDiffDlgFlag;
	int32	s32MaxFrameNum;
	CWnd	*pParentWnd;
	CImageDlg	*pCurrJumpImg;

	void	close_dlg();
// Dialog Data
	//{{AFX_DATA(CJumpFrameDlg)
	enum { IDD = IDD_JUMPFRAME_DIALOG };
	CButton	m_currJump;
	CEdit	m_frameNr;
	CString	m_sJumpFrameNr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJumpFrameDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CJumpFrameDlg)
	afx_msg void OnClear();
	afx_msg void OnAllJump();
	afx_msg void OnCurrJump();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void	initial();
	int32	get_frame_number();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JUMPFRAMEDLG_H__8062FC12_6A27_42DA_9052_F0D056107FC5__INCLUDED_)
