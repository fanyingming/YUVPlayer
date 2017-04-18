#if !defined(AFX_IMAGEDLG_H__5FB8C8B0_017E_4998_AF6E_99375D2D6664__INCLUDED_)
#define AFX_IMAGEDLG_H__5FB8C8B0_017E_4998_AF6E_99375D2D6664__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImageDlg.h : header file
//

#include "define.h"
#include "ChildDlg.h"
#include "CFile64.h"

/////////////////////////////////////////////////////////////////////////////
// CImageDlg dialog

class CImageDlg : public CChildDlg
{
// Construction
public:
    CImageDlg(CWnd* pParent = NULL);   // standard constructor
	CImageDlg(CWnd* pParent, CWnd* pMainDlg);

    uint8	bEOHFlag;		//++ 文件头标志
    uint8	bEOFFlag;		//++ 文件尾标志
    uint8	bForwardOK;		//++ 可前进标志
    uint8	bBackwardOK;	//++ 可后退标志
    uint8	bCloseFlag;	//++ 当前窗口是否被要求关闭
	uint8	u8MirrorMode;
	int16	s16RotateAngle;
    int32	s32SrcWidth;	//++ 图像原始宽度
    int32	s32SrcHeight;	//++ 图像原始高度
    int32	s32FrameNum;
    int32	s32StartFrameNr;
    int32	s32CurrFrameNr;
    CString	frameSize;
    CString	sampleFormat;
    CString zoomSize;
    CFile64	*pFile;

    void	free_resource();
    void	button_down_left();
    void	button_down_right();
    LRESULT	show_one_frame(WPARAM wParam, LPARAM lParam);
    int32	jump_to_frame(uint8 u8ImageMode, int32 s32JumpFrameNr);
    int32	create_window(CWnd *pParentWnd);

// Dialog Data
	//{{AFX_DATA(CImageDlg)
	enum { IDD = IDD_IMAGE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImageDlg)
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
    afx_msg void OnMenuitemGosameframe();
    afx_msg void OnMenuitemShowmbinfo();
	afx_msg void OnMenuitemNotice();
	afx_msg void OnMenuitemAttach();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg LRESULT OnEnterSizeMove(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnExitSizeMove(WPARAM wp, LPARAM lp);
	afx_msg void OnMenuitemMirrorHori();
	afx_msg void OnMenuitemMirrorVert();
	afx_msg void OnMenuitemRotate90();
	afx_msg void OnMenuitemRotate270();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
        
private:
    void	rotate_image(LPBYTE pSrcY, LPBYTE pSrcU, LPBYTE pSrcV);
    void	mirror_image(LPBYTE pSrcY, LPBYTE pSrcU, LPBYTE pSrcV);
    int32	initial();
    int32	read_one_frame(uint8 u8ImageMode);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEDLG_H__5FB8C8B0_017E_4998_AF6E_99375D2D6664__INCLUDED_)
