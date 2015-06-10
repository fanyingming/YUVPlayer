// YUVPlayerDlg.h : header file
//

#if !defined(AFX_YUVPLAYERDLG_H__5831D3F0_2ABA_4389_83C1_E575A0B60F7B__INCLUDED_)
#define AFX_YUVPLAYERDLG_H__5831D3F0_2ABA_4389_83C1_E575A0B60F7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "define.h"
#include "configure.h"
#include "DiffPicDlg.h"
#include "ImageDlg.h"
#include "CustomSizeDlg.h"
#include "JumpFrameDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CYUVPlayerDlg dialog

class CYUVPlayerDlg : public CDialog
{
// Construction
public:
	CYUVPlayerDlg(CWnd* pParent = NULL);	// standard constructor

	int8	s8ImgNum;
	int8	s8StopValue;		//++ 停止播放主值，标记哪个事件发出的停止播放请求
	int8	s8PlusValue;		//++ 停止播放副值，用于发出停止播放请求的事件给主程序传送附加信息
	uint8	bAttachFlag;		//++ 图像窗口是否粘附主窗口
	uint8	bNoticeFlag;		//++ 两图对比时，如果两图不相同，是否弹出提示
	uint8	bEnMBInfo;			//++ 是否使能宏块信息显示功能
	uint8	bShowMBInfo;		//++ 是否显示宏块信息
	uint8	bShowDiffPic;		//++ 是否显示差值图像窗口
	uint8	bReversePlay;		//++ 是否改变播放方向
	uint8	u8ImageMode;		//++ 图像显示模式
	uint8	u8PlayMode;			//++ 播放采用对比模式还是浏览模式
	uint8	u8EOFNum;			//++ 达到文件尾的图像个数
	uint8	u8EOHNum;			//++ 达到文件头的图像个数
    uint8   u8SampleFormat;
    int32	s32Width;			//++ 图像原始宽度
    int32	s32Height;			//++ 图像原始高度
    int32	s32ZoomWidth;		//++ 缩放后的宽度
    int32	s32ZoomHeight;		//++ 缩放后的高度
	int32	s32StartFrameNr;
	int32	s32MBInfoDlgX;
	int32	s32MBInfoDlgY;
	float	fFrameRate;
    CRect   cLayout;
	CString		m_sHeight;
	CString		m_sWidth;
	CConfigure	configure;
	CDiffPicDlg     diffPic;
	CRITICAL_SECTION	m_CriticalSection;
	CJumpFrameDlg*	pJumpFrameDlg;
	CImageDlg*		pImage[MAX_IMAGE_NUM];
	CWinThread*		m_pPlayThread;
	
	void	initial();
	void	set_comp_mode();
	void	set_view_mode();
	void	hide_MBinfo_dlg();
    void	layout_windows();
	void	close_image(int8 s8DlgIdx);
	LRESULT	show_one_frame(uint8 bReNotice);
	void	set_play_status(uint8 u8Status);
	void	adjust_window_position(CChildDlg *pCurrDlg);
    LRESULT    translate_message(WPARAM wParam, LPARAM lParam);
	uint8	get_play_status();

// Dialog Data
	//{{AFX_DATA(CYUVPlayerDlg)
	enum { IDD = IDD_YUVPLAYER_DIALOG };
	CEdit	m_frameRate;
	CComboBox	m_zoomSize;
	CComboBox	m_sampleFormat;
	CComboBox	m_frameSize;
	CButton	m_viewMode;
	CButton	m_compMode;
	CButton	m_resetPic;
	CButton	m_replay;
	CButton	m_play;
	CButton	m_openFile;
	CButton	m_fwOneStep;
	CButton	m_fwMultiStep;
	CButton	m_customSize;
	CButton	m_closePic;
	CButton	m_bkMultiStep;
	CButton	m_bkOneStep;
	CString	m_sFrameSize;
	CString	m_sSampleFormat;
	CString	m_sZoomSize;
	CString	m_sFrameRate;
	CString	m_sStartFrameNr;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CYUVPlayerDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	afx_msg void OnPlay();
	afx_msg void OnReplay();
	afx_msg void OnResetPic();
	afx_msg void OnClosePic();
	afx_msg void OnForwardOneStep();
	afx_msg void OnForwardMultistep();
	afx_msg void OnBackwardOneStep();
	afx_msg void OnBackwardMultistep();

protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CYUVPlayerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOpenFile();
	afx_msg void OnSetCustomSize();
	afx_msg void OnSetCompMode();
	afx_msg void OnSetViewMode();
	afx_msg void OnExit();
	afx_msg void OnClose();
	afx_msg void OnChangeFrameRate();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	uint8	u8PlayStatus;		//++ 播放状态

	void	open_files();
	void	drop_files(HDROP hDropInfo);
	void	exit_program();
	void	forward_one_step();
	void	forward_multistep(); 
	void	backward_one_step();
	void	backward_multistep();
	void	close_all_image();
	LRESULT	deal_stop_apply(WPARAM wParam, LPARAM lParam);
	LRESULT	adjust_image(WPARAM wParam, LPARAM lParam);
	void	set_button_status(uint8 u8Status);
	void	set_default_parameter_value();
	void	set_input_parameter_status(uint8 u8Status);
	void	change_frame_rate();
	int32	get_sample_ratio();
	int32	get_zoom_ratio();
	int32	get_frame_size();
	int32	get_input_parameter();
	int32	creat_image_window(CString CurrFilePath);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_YUVPLAYERDLG_H__5831D3F0_2ABA_4389_83C1_E575A0B60F7B__INCLUDED_)
