// YUVPlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include <cderr.h>
#include "YUVPlayerDlg.h"
#include "multithread.h"
#include "log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CYUVPlayerDlg dialog

CYUVPlayerDlg::CYUVPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CYUVPlayerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CYUVPlayerDlg)
	m_sSampleFormat		 = _T("YUV 4:2:0");
	m_sZoomSize			 = _T("100");
	m_sFrameRate		 = _T("30");
	m_sStartFrameNr		 = _T("1");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CYUVPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CYUVPlayerDlg)
	DDX_Control(pDX, IDC_FRAME_RATE, m_frameRate);
	DDX_Control(pDX, IDC_ZOOM, m_zoomSize);
	DDX_Control(pDX, IDC_SAMPLE_FORMAT, m_sampleFormat);
	DDX_Control(pDX, IDC_FRAME_SIZE, m_frameSize);
	DDX_Control(pDX, IDC_VIEW_MODE, m_viewMode);
	DDX_Control(pDX, IDC_COMP_MODE, m_compMode);
	DDX_Control(pDX, IDC_RESET_PIC, m_resetPic);
	DDX_Control(pDX, IDC_REPLAY, m_replay);
	DDX_Control(pDX, IDC_PLAY, m_play);
	DDX_Control(pDX, IDC_OPEN_FILE, m_openFile);
	DDX_Control(pDX, IDC_FORWARD_ONE_STEP, m_fwOneStep);
	DDX_Control(pDX, IDC_FORWARD_MULTISTEP, m_fwMultiStep);
	DDX_Control(pDX, IDC_CUSTOM_SIZE, m_customSize);
	DDX_Control(pDX, IDC_CLOSE_PIC, m_closePic);
	DDX_Control(pDX, IDC_BACKWARD_MULTISTEP, m_bkMultiStep);
	DDX_Control(pDX, IDC_BACKWARD_ONE_STEP, m_bkOneStep);
	DDX_CBString(pDX, IDC_FRAME_SIZE, m_sFrameSize);
	DDX_CBString(pDX, IDC_SAMPLE_FORMAT, m_sSampleFormat);
	DDX_CBString(pDX, IDC_ZOOM, m_sZoomSize);
	DDX_Text(pDX, IDC_FRAME_RATE, m_sFrameRate);
	DDX_Text(pDX, IDC_START_FRAME, m_sStartFrameNr);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CYUVPlayerDlg, CDialog)
	//{{AFX_MSG_MAP(CYUVPlayerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN_FILE, OnOpenFile)
	ON_BN_CLICKED(IDC_CUSTOM_SIZE, OnSetCustomSize)
	ON_BN_CLICKED(IDC_COMP_MODE, OnSetCompMode)
	ON_BN_CLICKED(IDC_VIEW_MODE, OnSetViewMode)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_WM_CLOSE()
	ON_EN_CHANGE(IDC_FRAME_RATE, OnChangeFrameRate)
	ON_WM_DROPFILES()
	ON_WM_MOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_REPLAY, OnReplay)
	ON_BN_CLICKED(IDC_FORWARD_ONE_STEP, OnForwardOneStep)
	ON_BN_CLICKED(IDC_FORWARD_MULTISTEP, OnForwardMultistep)
	ON_BN_CLICKED(IDC_BACKWARD_ONE_STEP, OnBackwardOneStep)
	ON_BN_CLICKED(IDC_BACKWARD_MULTISTEP, OnBackwardMultistep)
	ON_BN_CLICKED(IDC_CLOSE_PIC, OnClosePic)
	ON_BN_CLICKED(IDC_RESET_PIC, OnResetPic)
	ON_MESSAGE(WM_MYMESSAGE_0, deal_stop_apply)
	ON_MESSAGE(WM_MYMESSAGE_3, translate_message)
	ON_MESSAGE(WM_MYMESSAGE_4, adjust_image)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CYUVPlayerDlg message handlers

BOOL CYUVPlayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	SetWindowText("YUVPlayer 1.4");
	//++ 初始化临界区
	CCriticalSection::initial(&m_CriticalSection);
	//++ 打开配置文件，读入历史参数
	configure.read_configure_file();
	//++ 初始化差值图像
	diffPic.m_hWnd	= NULL;
	//++ 初始化主窗口控件及主程序参数
	initial();
	m_frameSize.SetCurSel(0);
	//++ 设置主窗口位置
	CRect	currRect;
	int32	s32ScreenWidth	 = GetSystemMetrics(SM_CXSCREEN);
	int32	s32ScreenHeight	 = GetSystemMetrics(SM_CYSCREEN);
	GetWindowRect(currRect);
    cLayout.left      = (s32ScreenWidth - currRect.Width()) >> 1;
    cLayout.top       = s32ScreenHeight >> 1;
    cLayout.right     = cLayout.left + currRect.Width();
    cLayout.bottom    = cLayout.top + currRect.Height();
	MoveWindow(cLayout, FALSE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CYUVPlayerDlg::initial()
{
#ifdef LOGINFO
	CLog("主窗口初始化");
#endif

	m_play.SetWindowText("播    放");
	m_replay.SetWindowText("回    放");
	
	set_input_parameter_status(TRUE);
	set_button_status(FALSE);

	bShowDiffPic		= FALSE;
	bAttachFlag			= TRUE;
	bNoticeFlag			= TRUE;
	u8PlayStatus		= STOP_STATUS;
	bEnMBInfo			= TRUE;
	bShowMBInfo			= FALSE;
	bReversePlay		= FALSE;
	u8ImageMode 		= IMAGE_YUV;
	s8StopValue			= VALUE_INVALID;
	s8PlusValue			= VALUE_INVALID;
	u8PlayMode			= VIEW_MODE;
	s8ImgNum			= 0;
	u8EOFNum			= 0;
	u8EOHNum			= 0;
	s32MBInfoDlgX		= 0;
	s32MBInfoDlgY		= 0;
	pJumpFrameDlg		= NULL;
	m_pPlayThread		= NULL;
	m_compMode.SetCheck(BST_UNCHECKED);
	m_viewMode.SetCheck(BST_CHECKED);
	m_viewMode.SetFocus();

	for (int32 s32ImgIdx = 0; s32ImgIdx < MAX_IMAGE_NUM; s32ImgIdx ++)
	{
		pImage[s32ImgIdx]	 = NULL;
	}

	//++ 销毁差值图像窗口
	if (diffPic.m_hWnd != NULL)
	{
		diffPic.kill_window();
	}
	bShowDiffPic	= FALSE;
}

void CYUVPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CYUVPlayerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CYUVPlayerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CYUVPlayerDlg::OnOpenFile() 
{
	// TODO: Add your control notification handler code here
	if (get_play_status() != PLAY_STATUS)
	{
		open_files();
	}
	else
	{
		s8StopValue	 = PAUSE_VALUE_9;
		set_play_status(PAUSE_STATUS);
	}
}

void CYUVPlayerDlg::open_files() 
{
	// TODO: Add your control notification handler code here
	int32			i;
	int32			s32Ret;
	int32			s32fileNum		 = 1;
	const uint32	u32MaxBuffLen	 = 256 * MAX_IMAGE_NUM;
	int8			a8Buff[u32MaxBuffLen];
	CString			szFilter		 = "*.yuv|*.yuv|All Files (*.*)|*.*||";
	CStringArray	fileList;
	POSITION		pos;
	CFileDialog		dlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY, szFilter, this);

	
	dlg.m_ofn.nMaxFile		 = u32MaxBuffLen;   
	dlg.m_ofn.lpstrFile		 = a8Buff;   
	dlg.m_ofn.lpstrFile[0]   = NULL;   
	if(dlg.DoModal() == IDOK)
	{
		pos   =   dlg.GetStartPosition();   
		while(pos)   
		{   
			CString  CurrFilePath   =   dlg.GetNextPathName(pos); 
			fileList.Add(CurrFilePath);
		}  
	}
	else
	{
		s32Ret	 = CommDlgExtendedError();
		if (s32Ret == FNERR_BUFFERTOOSMALL)
		{
			AfxMessageBox("选择的文件数过多，当前版本只支持最多 10 幅图像！", MB_ICONERROR);
		}
		
		return;
	}

	s32fileNum	 = fileList.GetSize();
	if (u8PlayMode == VIEW_MODE)
	{
		if ((s32fileNum + s8ImgNum) > MAX_IMAGE_NUM)
		{
			AfxMessageBox("多图浏览模式最多只允许打开 10 幅图像！", MB_ICONERROR);
			
			return;
		}
	}
	else
	{
		if ((s32fileNum + s8ImgNum) > 2)
		{
			s32Ret	= AfxMessageBox("两图对比模式只允许打开 2 幅图像，是否切换为多图浏览模式？", MB_YESNO | MB_ICONINFORMATION);
			if (s32Ret == IDNO)
			{
				return;
			}

			if ((s32fileNum + s8ImgNum) > MAX_IMAGE_NUM)
			{
				AfxMessageBox("多图浏览模式最多只允许打开 10 幅图像！", MB_ICONERROR);
				
				return;
			}

			set_view_mode();
		}
	}
	
	s32Ret	 = get_input_parameter();
	if (s32Ret == FAILED_YUVPlayer)
	{
		return;
	}

	for (i = 0; i < s32fileNum; i ++)
	{
		s32Ret	 = creat_image_window(fileList.GetAt(i));
		if (s32Ret == SUCCEEDED_YUVPlayer)
		{
			if (u8PlayMode == COMP_MODE)
			{
				if (s8ImgNum == 1)
				{
					set_input_parameter_status(FALSE);
//					AfxMessageBox("现在使用的是两图对比模式，请再打开一幅原始尺寸相同的图像！", MB_ICONINFORMATION);
					m_openFile.SetFocus();
				}
				else
				{
					diffPic.create_window(this);
					diffPic.show_window();
				}
			}
		}
	}
}

void CYUVPlayerDlg::drop_files(HDROP hDropInfo)
{
    int8	filePath[256];
	int32	s32fileNum;
	int32	i;
    int32	s32Ret;

	
	s32fileNum	 = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	if (u8PlayMode == VIEW_MODE)
	{
		if ((s32fileNum + s8ImgNum) > MAX_IMAGE_NUM)
		{
			AfxMessageBox("多图浏览模式最多只允许打开 10 幅图像！", MB_ICONERROR);
			
			return;
		}
	}
	else
	{
		if ((s32fileNum + s8ImgNum) > 2)
		{
			s32Ret	= AfxMessageBox("两图对比模式只允许打开 2 幅图像，是否切换为多图浏览模式？", MB_YESNO | MB_ICONINFORMATION);
			if (s32Ret == IDNO)
			{
				return;
			}
			
			if ((s32fileNum + s8ImgNum) > MAX_IMAGE_NUM)
			{
				AfxMessageBox("多图浏览模式最多只允许打开 10 幅图像！", MB_ICONERROR);
				
				return;
			}

			set_view_mode();
		}
	}
	
	s32Ret	 = get_input_parameter();
	if (s32Ret == FAILED_YUVPlayer)
	{
		return;
	}
	
	for (i = 0; i < s32fileNum; i ++)
	{
		DragQueryFile(hDropInfo, i, filePath, 256);
		s32Ret	 = creat_image_window(filePath);
		if (s32Ret == SUCCEEDED_YUVPlayer)
		{
			if (u8PlayMode == COMP_MODE)
			{
				if (s8ImgNum == 1)
				{
					set_input_parameter_status(FALSE);
//					AfxMessageBox("现在使用的是两图对比模式，请再打开一幅原始尺寸相同的图像！", MB_ICONINFORMATION);
					m_openFile.SetFocus();
				}
				else
				{
					diffPic.create_window(this);
					diffPic.show_window();
				}
			}
		}
	}
}

void CYUVPlayerDlg::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	//++ 启用临界区保护
	CCriticalSection	CriticalSection(&m_CriticalSection);

	drop_files(hDropInfo);
	DragFinish(hDropInfo);

	CDialog::OnDropFiles(hDropInfo);
}

int32 CYUVPlayerDlg::creat_image_window(CString CurrFilePath)
{
    int32	s32Ret;
    CFile64	*pCurrFile	= new CFile64();


	if(!pCurrFile->Open(CurrFilePath, CFile::shareDenyWrite)) 
	{
		delete(pCurrFile);
		AfxMessageBox("打开文件失败！", MB_ICONERROR);

		return FAILED_YUVPlayer;
	}

	CImageDlg	*pNewImg		= new CImageDlg((CWnd *)this, (CWnd *)this);
	pNewImg->bEOHFlag			= FALSE;
	pNewImg->bEOFFlag			= FALSE;
	pNewImg->bForwardOK			= TRUE;
	pNewImg->bBackwardOK		= TRUE;
	pNewImg->bCloseFlag			= FALSE;
	pNewImg->u8MirrorMode		= MIRROR_NONE;
	pNewImg->s16RotateAngle		= 0;
	pNewImg->s32StartFrameNr	= s32StartFrameNr;
	pNewImg->s32CurrFrameNr		= s32StartFrameNr;
	pNewImg->frameSize			= m_sFrameSize;
	pNewImg->sampleFormat		= m_sSampleFormat;
	pNewImg->zoomSize			= m_sZoomSize;
	pNewImg->pFile				= pCurrFile;
	pNewImg->s8DlgIdx			= s8ImgNum;
    pNewImg->u8SampleFormat		= u8SampleFormat;
	pNewImg->s32SrcWidth		= s32Width;
	pNewImg->s32SrcHeight		= s32Height;
	pNewImg->s32Width			= s32Width;
	pNewImg->s32Height			= s32Height;
	pNewImg->s32ZoomWidth		= s32ZoomWidth;
	pNewImg->s32ZoomHeight		= s32ZoomHeight;
	pNewImg->pCriticalSection	= &(m_CriticalSection);
	pNewImg->u32LumaPicSize		= s32Width * s32Height;
	pNewImg->s32MBXNum			= (s32Width + 15) >> 4;
	pNewImg->s32MBYNum			= (s32Height + 15) >> 4;

#ifdef LOGINFO
	CLog("创建图像窗口>>开始 " + CurrFilePath);
#endif

	s32Ret	 = pNewImg->create_window((CWnd *)this);
	if (s32Ret == FAILED_YUVPlayer)
	{
		if (pCurrFile->m_hFile != pCurrFile->hFileNull)
		{
			pCurrFile->Close();
		}

		delete(pNewImg);

		return FAILED_YUVPlayer;
	}

	pNewImg->ShowWindow(SW_SHOW);
	pImage[s8ImgNum ++]	 = pNewImg;
	set_button_status(TRUE);
	SetFocus();
#ifdef LOGINFO
	CLog("创建图像窗口>>完成 " + CurrFilePath);
#endif

	return SUCCEEDED_YUVPlayer;
}

void CYUVPlayerDlg::set_default_parameter_value()
{
	s32Width		= pImage[0]->s32Width;
	s32Height		= pImage[0]->s32Height;

	m_sFrameSize	= pImage[0]->frameSize;
	m_sSampleFormat	= pImage[0]->sampleFormat;
	m_sZoomSize		= pImage[0]->zoomSize;
	
	UpdateData(FALSE);
}

int32 CYUVPlayerDlg::get_input_parameter()
{
#ifdef LOGINFO
	CLog("读取输入参数");
#endif
	UpdateData(TRUE);

	if ((u8PlayMode == VIEW_MODE) || (s8ImgNum == 0))
	{
		int32	s32Ret;

		s32Ret	= get_sample_ratio();
		if (s32Ret == FAILED_YUVPlayer)
		{
			return FAILED_YUVPlayer;
		}
		
		s32Ret	= get_frame_size();
		if (s32Ret == FAILED_YUVPlayer)
		{
			return FAILED_YUVPlayer;
		}

		s32Ret	= get_zoom_ratio();
		if (s32Ret == FAILED_YUVPlayer)
		{
			return FAILED_YUVPlayer;
		}
	}
	else
	{
		s32Width		= pImage[0]->s32Width;
		s32Height		= pImage[0]->s32Height;
		u8SampleFormat	= pImage[0]->u8SampleFormat;
	}

	fFrameRate			 = atof(m_sFrameRate);
	//++ 检查输入参数
	if ((fFrameRate < 1.0) || fFrameRate > 1000.0)
	{
		AfxMessageBox("帧率无效！请输入 1 到 1000 之间的数字。", MB_ICONERROR);
		
		return FAILED_YUVPlayer;
	}
	
	s32StartFrameNr		 = atoi(m_sStartFrameNr);
	if (s32StartFrameNr <= 0)
	{
		AfxMessageBox("开始播放帧号无效！", MB_ICONERROR);
		
		return FAILED_YUVPlayer;
	}

	UpdateData(FALSE);

	return SUCCEEDED_YUVPlayer;
}

int32 CYUVPlayerDlg::get_sample_ratio()
{
	int32	s32ItemIdx	 = m_sampleFormat.GetCurSel();

	switch(s32ItemIdx)
	{
	case 0:
		u8SampleFormat   = YUV400;
		break;

	case 1:
		u8SampleFormat   = YUV420;
		break;

	default:
		AfxMessageBox("采样格式无效！", MB_ICONERROR);
		return FAILED_YUVPlayer;
	    break;
	}

	return SUCCEEDED_YUVPlayer;
}

int32 CYUVPlayerDlg::get_zoom_ratio() 
{
	// TODO: Add your control notification handler code here
	int32	s32ScreenWidth		= GetSystemMetrics(SM_CXSCREEN);
	int32	s32ScreenHeight		= GetSystemMetrics(SM_CYSCREEN) - 64;
	int32	s32ItemIdx		= m_zoomSize.GetCurSel();
	
	switch(s32ItemIdx)
	{
	case 0:
		s32ZoomWidth	= s32Width >> 2;
		s32ZoomHeight	= s32Height >> 2;
		m_sZoomSize		= _T(" 25%");
		break;
		
	case 1:
		s32ZoomWidth	= s32Width >> 1;
		s32ZoomHeight	= s32Height >> 1;
		m_sZoomSize		= _T(" 50%");
		break;
		
	case 2:
		s32ZoomWidth	= s32Width;
		s32ZoomHeight	= s32Height;
		m_sZoomSize		= _T("100%");
		break;
		
	case 3:
		s32ZoomWidth	= s32Width << 1;
		s32ZoomHeight	= s32Height << 1;
		m_sZoomSize		= _T("200%");
		break;
		
	case 4:
		s32ZoomWidth	= s32Width << 2;
		s32ZoomHeight	= s32Height << 2;
		m_sZoomSize		= _T("400%");
		break;
		
	default:
		AfxMessageBox("缩放比例无效！", MB_ICONERROR);
		return FAILED_YUVPlayer;
		break;
	}

	if (s32ZoomWidth >= s32ScreenWidth)
	{
		s32ZoomHeight	= s32Height * s32ScreenWidth / s32Width;
		s32ZoomWidth	= s32ScreenWidth;
	}
	if (s32ZoomHeight >= s32ScreenHeight)
	{
		s32ZoomWidth	= s32Width * s32ScreenHeight / s32Height;
		s32ZoomHeight	= s32ScreenHeight;
	}

	return SUCCEEDED_YUVPlayer;
}

int32 CYUVPlayerDlg::get_frame_size() 
{
	// TODO: Add your control notification handler code here
	int32	s32ItemIdx;
	int32	s32Pos;
	int32	s32Len;


	s32Pos		 = m_sFrameSize.Find('x');
	s32Len		 = m_sFrameSize.GetLength();
	s32Width	 = atoi(m_sFrameSize.Left(s32Pos));
	s32Height	 = atoi(m_sFrameSize.Mid(s32Pos + 1, s32Len - s32Pos - 1));
	if ((s32Height <= 0) || s32Width <= 0)
	{
		AfxMessageBox("图像尺寸无效！", MB_ICONERROR);
		
		return FAILED_YUVPlayer;
	}

	s32ItemIdx		 = m_frameSize.FindString(-1, m_sFrameSize);
	if (s32ItemIdx >= 0)
	{
		configure.modify_priority(s32ItemIdx);
	}
	else
	{
		//++ 将数据更新到主窗口列表中
		m_frameSize.AddString(m_sFrameSize);
		//++ 将数据更新到配置文件缓存中
		configure.append_item(0, m_sFrameSize);
	}

	return SUCCEEDED_YUVPlayer;
}

void CYUVPlayerDlg::OnSetCustomSize() 
{
	// TODO: Add your control notification handler code here
	m_frameSize.EnableWindow(FALSE);
	//++ 创建自定义窗口
	CCustomSizeDlg	customSizeDlg((CWnd *)this, (CWnd *)this);
	
	customSizeDlg.DoModal();
}

void CYUVPlayerDlg::set_input_parameter_status(uint8 u8Status)
{
#ifdef LOGINFO
	CLog("设置输入参数状态");
#endif

	m_zoomSize.EnableWindow(u8Status);
	m_frameSize.EnableWindow(u8Status);
	m_sampleFormat.EnableWindow(u8Status);
}

void CYUVPlayerDlg::set_button_status(uint8 u8Status)
{
#ifdef LOGINFO
	CLog("设置控件状态");
#endif
	
	m_play.EnableWindow(u8Status);
	m_replay.EnableWindow(u8Status);
	m_resetPic.EnableWindow(u8Status);
	m_closePic.EnableWindow(u8Status);
	m_fwOneStep.EnableWindow(u8Status);
	m_bkOneStep.EnableWindow(u8Status);
	m_bkMultiStep.EnableWindow(u8Status);
	m_fwMultiStep.EnableWindow(u8Status);

	u8PlayStatus	 = u8Status;
}

void CYUVPlayerDlg::set_play_status(uint8 u8Status)
{
	//++ 启用临界区保护
	CCriticalSection	CriticalSection(&m_CriticalSection);

	u8PlayStatus	 = u8Status;
}

uint8 CYUVPlayerDlg::get_play_status()
{
	//++ 启用临界区保护
	CCriticalSection	CriticalSection(&m_CriticalSection);

	return u8PlayStatus;
}

void CYUVPlayerDlg::OnSetViewMode() 
{
	// TODO: Add your control notification handler code here
	if (u8PlayMode == VIEW_MODE)
	{
		return;
	}
	
	if (get_play_status() != PLAY_STATUS)
	{
		set_view_mode();
	}
	else
	{
		s8StopValue	 = PAUSE_VALUE_7;
		set_play_status(PAUSE_STATUS);
	}
}

void CYUVPlayerDlg::set_view_mode() 
{
	// TODO: Add your control notification handler code here
 	m_compMode.SetCheck(BST_UNCHECKED);
 	m_viewMode.SetCheck(BST_CHECKED);
 	m_viewMode.SetFocus();
	u8PlayMode		= VIEW_MODE;

	if (diffPic.m_hWnd != NULL)
	{
		diffPic.kill_window();
	}
	bShowDiffPic	= FALSE;
	
	set_input_parameter_status(TRUE);
}

void CYUVPlayerDlg::OnSetCompMode() 
{
	// TODO: Add your control notification handler code here
	if (u8PlayMode == COMP_MODE)
	{
		return;
	}

	if (get_play_status() != PLAY_STATUS)
	{
		set_comp_mode();
	}
	else
	{
		s8StopValue	 = PAUSE_VALUE_8;
		set_play_status(PAUSE_STATUS);
	}
}

void CYUVPlayerDlg::set_comp_mode() 
{
	// TODO: Add your control notification handler code here
 	u8PlayMode	 = COMP_MODE;
 	m_viewMode.SetCheck(BST_UNCHECKED);
	m_compMode.SetCheck(BST_CHECKED);
	m_compMode.SetFocus();

	if (s8ImgNum == 1)
	{
		m_openFile.SetFocus();
//		AfxMessageBox("现在使用的是两图对比模式，请再打开一幅原始尺寸相同的图像！", MB_ICONINFORMATION);
		set_input_parameter_status(FALSE);
		set_default_parameter_value();

		return;
	}
	else if (s8ImgNum == 2)
	{
		if ((pImage[0]->s32SrcWidth != pImage[1]->s32SrcWidth) || (pImage[0]->s32SrcHeight != pImage[1]->s32SrcHeight))
		{
			AfxMessageBox("两幅图像原始尺寸必须相同！", MB_ICONERROR);
		}
        else if (pImage[0]->u8SampleFormat != pImage[1]->u8SampleFormat)
        {
            AfxMessageBox("两幅图像采样格式必须一致！", MB_ICONERROR);
        }
        else if ((pImage[0]->s16RotateAngle != pImage[1]->s16RotateAngle))
        {
            AfxMessageBox("两幅图像旋转角度必须一致！", MB_ICONERROR);
        }
		else
		{
			set_input_parameter_status(FALSE);
			set_default_parameter_value();
			adjust_window_position(pImage[0]);
			adjust_window_position(pImage[1]);

			if (diffPic.create_window(this) == SUCCEEDED_YUVPlayer)
			{
				diffPic.show_window();
			}

			return;
		}
		
		set_view_mode();
	}
	else if (s8ImgNum > 2)
	{
		AfxMessageBox("两图对比模式只允许打开 2 幅图像，请关掉多余图像！", MB_ICONERROR);
		
		set_view_mode();
	}
}

void CYUVPlayerDlg::adjust_window_position(CChildDlg *pCurrDlg)
{
	int32	s32WndWidth;
	int32	s32WndHeight;
	int32	s32ScreenWidth	 = GetSystemMetrics(SM_CXSCREEN);
	int32	s32ScreenHeight	 = GetSystemMetrics(SM_CYSCREEN);
	int32	s32MaxMargin;
	CRect   currRect;
	CRect	mainRect;

	
    //++ 获取主窗口位置
    GetWindowRect(mainRect);
    //++ 获取当前窗口位置
    pCurrDlg->GetWindowRect(currRect);

	if (pCurrDlg->s8DlgIdx > 0)
	{
		CRect	prevRect;

		pImage[pCurrDlg->s8DlgIdx - 1]->GetWindowRect(prevRect);
		s32WndWidth		 = currRect.Width();
		s32WndHeight	 = currRect.Height();
        currRect.bottom	 = prevRect.bottom;
        currRect.top	 = prevRect.bottom - s32WndHeight;
        currRect.left	 = prevRect.right;
        currRect.right	 = prevRect.right + s32WndWidth;

        //++ 图像超出右边界或右边已经有图像，则调整图像位置
        if ((currRect.right > s32ScreenWidth) || (currRect.left < cLayout.right))
        {
            if (cLayout.left > s32WndWidth)	//++ 检查左边空间
            {
                currRect.right	 = cLayout.left;
                currRect.left	 = cLayout.left - s32WndWidth;
            }
            else if (cLayout.top > s32WndHeight)	//++ 检查上边空间
            {
				currRect.left	 = cLayout.left;
				currRect.right	 = cLayout.left + s32WndWidth;
				currRect.bottom	 = cLayout.top;
				currRect.top	 = cLayout.top - s32WndHeight;
				//++ 换行后重新设置右边界
				cLayout.right    = currRect.right;
			}
			else
			{
				s32MaxMargin	= max(s32ScreenWidth - cLayout.right, max(cLayout.top, cLayout.left));
				if (s32MaxMargin == cLayout.top)
				{
					currRect.left	 = cLayout.left;
					currRect.right	 = cLayout.left + s32WndWidth;
					currRect.bottom	 = cLayout.top;
					currRect.top	 = cLayout.top - s32WndHeight;
					//++ 换行后重新设置右边界
					cLayout.right    = currRect.right;
				}
				else if (s32MaxMargin == cLayout.left)
				{
					currRect.right	 = cLayout.left;
					currRect.left	 = cLayout.left - s32WndWidth;
				}
				else
				{
					currRect.left	 = cLayout.right;
					currRect.right	 = cLayout.right + s32WndWidth;
				}
			}
        }

		cLayout.top       = min(cLayout.top, currRect.top);
		cLayout.left      = min(cLayout.left, currRect.left);
		cLayout.bottom    = max(cLayout.bottom, currRect.bottom);
		cLayout.right     = max(cLayout.right, currRect.right);
		pCurrDlg->MoveWindow(currRect);
	}
	else
	{
		s32WndWidth		 = currRect.Width();
		s32WndHeight	 = currRect.Height();

		//++ 如果主窗口位于屏幕下半边，则将图像对齐到主窗口上边缘
		currRect.left		= mainRect.left;
		currRect.top		= mainRect.top - s32WndHeight;
		currRect.right		= mainRect.left + s32WndWidth;
		currRect.bottom		= mainRect.top;
        cLayout.top			= min(mainRect.top, currRect.top);
        cLayout.left		= min(mainRect.left, currRect.left);
        cLayout.bottom		= max(mainRect.bottom, currRect.bottom);
        cLayout.right		= currRect.right;
		pCurrDlg->wndRect	= currRect;
		pCurrDlg->MoveWindow(currRect);
	}
}

void CYUVPlayerDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	OnExit();
}

void CYUVPlayerDlg::OnExit() 
{
	// TODO: Add your control notification handler code here
	if (get_play_status() != PLAY_STATUS)
	{
		exit_program();
	}
	else
	{
		s8StopValue	 = PAUSE_VALUE_0;
		set_play_status(PAUSE_STATUS);
	}
}

void CYUVPlayerDlg::exit_program()
{
	if (diffPic.m_hWnd != NULL)
	{
		diffPic.kill_window();
	}
	bShowDiffPic	= FALSE;

	//++ 删除临界区
	CCriticalSection::delet(&m_CriticalSection);

	configure.write_configure_file();

	CDialog::OnCancel();
}

void CYUVPlayerDlg::OnPlay() 
{
    // TODO: Add your control notification handler code here
	if ((bReversePlay == TRUE) || (get_play_status() != PLAY_STATUS))
	{
		m_play.SetWindowText("暂    停");
		set_play_status(PLAY_STATUS);

		//++ 播放过程中隐藏宏块信息窗口，并不再显示宏块信息
		bShowMBInfo	= FALSE;
		hide_MBinfo_dlg();

		if (m_pPlayThread == NULL)
		{
			m_pPlayThread	 = AfxBeginThread((AFX_THREADPROC)play_video, (void*)this);
		}
	}
	else
	{
		m_play.SetWindowText("继续播放");
		s8StopValue	 = VALUE_INVALID;
		s8PlusValue	 = VALUE_INVALID;
		set_play_status(PAUSE_STATUS);
	}

	bReversePlay	 = FALSE;
	m_replay.SetWindowText("回    放");
}

void CYUVPlayerDlg::OnReplay() 
{
    // TODO: Add your control notification handler code here
	if ((bReversePlay == FALSE) || (get_play_status() != PLAY_STATUS))
	{
		m_replay.SetWindowText("暂    停");
		set_play_status(PLAY_STATUS);

		//++ 播放过程中隐藏宏块信息窗口，并不再显示宏块信息
		bShowMBInfo	= FALSE;
		hide_MBinfo_dlg();

		if (m_pPlayThread == NULL)
		{
			m_pPlayThread	 = AfxBeginThread((AFX_THREADPROC)play_video, (void*)this);
		}
	}
	else
	{
		m_replay.SetWindowText("继续回放");
		s8StopValue	 = VALUE_INVALID;
		s8PlusValue	 = VALUE_INVALID;
		set_play_status(PAUSE_STATUS);
	}
	
	bReversePlay	 = TRUE;
	m_play.SetWindowText("播    放");
}

void CYUVPlayerDlg::change_frame_rate()
{
	float	fNewFrameRate;

	
	UpdateData(TRUE);
	fNewFrameRate	 = atof(m_sFrameRate);
	if ((fNewFrameRate < 1) || fNewFrameRate > 1000)
	{
		AfxMessageBox("输入帧率无效！请输入 1 到 1000 之间的数字。", MB_ICONERROR);
		m_sFrameRate	 = _T("");
		UpdateData(FALSE);
	}
	else
	{
		fFrameRate	 = fNewFrameRate;
	}
}

void CYUVPlayerDlg::OnChangeFrameRate() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if (get_play_status() != PLAY_STATUS)
	{
		change_frame_rate();
	}
	else
	{
		s8StopValue		= PAUSE_VALUE_14;
		set_play_status(PAUSE_STATUS);
	}
}

void CYUVPlayerDlg::OnForwardOneStep() 
{
	// TODO: Add your control notification handler code here
	if (get_play_status() != PLAY_STATUS)
	{
		forward_one_step();
	}
	else
	{
		s8StopValue		= PAUSE_VALUE_1;
		set_play_status(PAUSE_STATUS);
	}
}

void CYUVPlayerDlg::forward_one_step() 
{
	int32	s32ImgIdx;


	m_fwOneStep.SetFocus();
	u8EOHNum	 = 0;

	if (u8EOFNum != s8ImgNum)
	{
		for (s32ImgIdx = 0; s32ImgIdx < s8ImgNum; s32ImgIdx ++)
		{
			CImageDlg	*pCurrImg	 = pImage[s32ImgIdx];

			pCurrImg->bEOHFlag		 = FALSE;
			pCurrImg->bBackwardOK	 = TRUE;
			if (pCurrImg->bEOFFlag == FALSE)
			{
				if ((pCurrImg->s32CurrFrameNr + 1) <= pCurrImg->s32FrameNum)
				{
					pCurrImg->s32CurrFrameNr ++;
					pCurrImg->show_one_frame(u8ImageMode, TRUE);
					pCurrImg->show_macroblock_info();
				}
				
				if (pCurrImg->s32CurrFrameNr == pCurrImg->s32FrameNum)
				{
					pCurrImg->bForwardOK	 = FALSE;
					pCurrImg->bEOFFlag		 = TRUE;
					u8EOFNum ++;
//					AfxMessageBox(pCurrImg->fileName + " 已经到达尾帧！", MB_ICONINFORMATION);
				}
			}
		}

		if (bShowDiffPic == TRUE)
		{
			diffPic.show_one_frame(u8ImageMode, TRUE);
			diffPic.show_macroblock_info();
		}
	}

	if (u8EOFNum == s8ImgNum)
	{
		AfxMessageBox("所有图像已经到达尾帧！", MB_ICONINFORMATION);
	}
}

void CYUVPlayerDlg::OnForwardMultistep() 
{
	// TODO: Add your control notification handler code here
	if (get_play_status() != PLAY_STATUS)
	{
		forward_multistep();
	}
	else
	{
		s8StopValue	 = PAUSE_VALUE_2;
		set_play_status(PAUSE_STATUS);
	}
}

void CYUVPlayerDlg::forward_multistep() 
{
	uint8	u8StepOK	 = FALSE;
	int32	s32ImgIdx;


	m_fwMultiStep.SetFocus();
	u8EOHNum	 = 0;

	for (s32ImgIdx = 0; s32ImgIdx < s8ImgNum; s32ImgIdx ++)
	{
		u8StepOK	|= pImage[s32ImgIdx]->bForwardOK;
	}
	if (u8StepOK == TRUE)
	{
		for (s32ImgIdx = 0; s32ImgIdx < s8ImgNum; s32ImgIdx ++)
		{
			CImageDlg	*pCurrImg	 = pImage[s32ImgIdx];

			pCurrImg->bEOHFlag		 = FALSE;
			pCurrImg->bBackwardOK	 = TRUE;
			if (pCurrImg->bForwardOK == TRUE)
			{
				if ((pCurrImg->s32CurrFrameNr + 5) <= pCurrImg->s32FrameNum)
				{
					pCurrImg->pFile->Seek(4 * pCurrImg->u32FrameSize, CFile::current);
					pCurrImg->s32CurrFrameNr	+= 5;
					pCurrImg->show_one_frame(u8ImageMode, TRUE);
					pCurrImg->show_macroblock_info();
				}
				else if (pCurrImg->s32CurrFrameNr != pCurrImg->s32FrameNum)
				{
					pCurrImg->bForwardOK	 = FALSE;
//					AfxMessageBox(pCurrImg->fileName + " 已经到达最后五帧\r\n仍可单帧步进！", MB_ICONINFORMATION);
				}
				
				if (pCurrImg->s32CurrFrameNr == pCurrImg->s32FrameNum)
				{
					pCurrImg->bForwardOK	 = FALSE;
					pCurrImg->bEOFFlag		 = TRUE;
					u8EOFNum ++;
//					AfxMessageBox(pCurrImg->fileName + " 已经到达尾帧！", MB_ICONINFORMATION);
				}
			}
		}

		if (bShowDiffPic == TRUE)
		{
			diffPic.show_one_frame(u8ImageMode, TRUE);
			diffPic.show_macroblock_info();
		}

		if (u8EOFNum == s8ImgNum)
		{
			AfxMessageBox("所有图像已经到达尾帧！", MB_ICONINFORMATION);
		}
	}
}

void CYUVPlayerDlg::OnBackwardOneStep() 
{
	// TODO: Add your control notification handler code here
	if (get_play_status() != PLAY_STATUS)
	{
		backward_one_step();
	}
	else
	{
		s8StopValue	 = PAUSE_VALUE_3;
		set_play_status(PAUSE_STATUS);
	}
}

void CYUVPlayerDlg::backward_one_step() 
{
	int32	s32ImgIdx;


	m_bkOneStep.SetFocus();
	u8EOFNum	 = 0;

	if (u8EOHNum != s8ImgNum)
	{
		for (s32ImgIdx = 0; s32ImgIdx < s8ImgNum; s32ImgIdx ++)
		{
			CImageDlg	*pCurrImg	 = pImage[s32ImgIdx];

			pCurrImg->bEOFFlag		 = FALSE;
			pCurrImg->bForwardOK	 = TRUE;
			if (pCurrImg->bEOHFlag == FALSE)
			{
				if ((pCurrImg->s32CurrFrameNr - 1) >= 1)
				{
					pCurrImg->pFile->Seek(-2 * pCurrImg->u32FrameSize, CFile::current);
					pCurrImg->s32CurrFrameNr --;
					pCurrImg->show_one_frame(u8ImageMode, TRUE);
					pCurrImg->show_macroblock_info();
				}

				if (pCurrImg->s32CurrFrameNr == 1)
				{
					pCurrImg->bEOHFlag		 = TRUE;
					pCurrImg->bBackwardOK	 = FALSE;
					u8EOHNum ++;
//					AfxMessageBox(pCurrImg->fileName + " 已经到达首帧！", MB_ICONINFORMATION);
				}
			}
		}

		if (bShowDiffPic == TRUE)
		{
			diffPic.show_one_frame(u8ImageMode, TRUE);
			diffPic.show_macroblock_info();
		}
	}

	if (u8EOHNum == s8ImgNum)
	{
		AfxMessageBox("所有图像已经到达首帧！", MB_ICONINFORMATION);
	}
}

void CYUVPlayerDlg::OnBackwardMultistep() 
{
	// TODO: Add your control notification handler code here
	if (get_play_status() != PLAY_STATUS)
	{
		backward_multistep();
	}
	else
	{
		s8StopValue	 = PAUSE_VALUE_4;
		set_play_status(PAUSE_STATUS);
	}
}

void CYUVPlayerDlg::backward_multistep()
{
	uint8	u8StepOK	 = FALSE;
	int32	s32ImgIdx;


	m_bkMultiStep.SetFocus();
	u8EOFNum	 = 0;

	for (s32ImgIdx = 0; s32ImgIdx < s8ImgNum; s32ImgIdx ++)
	{
		u8StepOK	|= pImage[s32ImgIdx]->bBackwardOK;
	}
	if (u8StepOK == TRUE)
	{
		for (s32ImgIdx = 0; s32ImgIdx < s8ImgNum; s32ImgIdx ++)
		{
			CImageDlg	*pCurrImg	 = pImage[s32ImgIdx];

			pCurrImg->bEOFFlag		 = FALSE;
			pCurrImg->bForwardOK	 = TRUE;
			if (pCurrImg->bBackwardOK == TRUE)
			{
				if ((pCurrImg->s32CurrFrameNr - 5) >= 1)
				{
					pCurrImg->pFile->Seek(-6 * pCurrImg->u32FrameSize, CFile::current);
					pCurrImg->s32CurrFrameNr	-= 5;
					pCurrImg->show_one_frame(u8ImageMode, TRUE);
					pCurrImg->show_macroblock_info();
				}
				else if (pCurrImg->s32CurrFrameNr != 1)
				{
					pCurrImg->bBackwardOK	 = FALSE;
//					AfxMessageBox(pCurrImg->fileName + " 已经到达最前五帧\r\n仍可单帧步退！", MB_ICONINFORMATION);
				}
				
				if (pCurrImg->s32CurrFrameNr == 1)
				{
					pCurrImg->bBackwardOK	 = FALSE;
					pCurrImg->bEOHFlag		 = TRUE;
					u8EOHNum ++;
//					AfxMessageBox(pCurrImg->fileName + " 已经到达首帧！", MB_ICONINFORMATION);
				}
			}
		}

		if (bShowDiffPic == TRUE)
		{
			diffPic.show_one_frame(u8ImageMode, TRUE);
			diffPic.show_macroblock_info();
		}

		if (u8EOHNum == s8ImgNum)
		{
			AfxMessageBox("所有图像已经到达首帧！", MB_ICONINFORMATION);
		}
	}
}

void CYUVPlayerDlg::OnClosePic() 
{
	// TODO: Add your control notification handler code here
	if (get_play_status() != PLAY_STATUS)
	{
		close_all_image();
	}
	else
	{
		s8StopValue	 = PAUSE_VALUE_6;
		set_play_status(PAUSE_STATUS);
	}
}

void CYUVPlayerDlg::close_all_image()
{
	for (int32 s32ImgIdx = 0; s32ImgIdx < s8ImgNum; s32ImgIdx ++)
	{
		pImage[s32ImgIdx]->bCloseFlag	 = TRUE;
	}

	close_image(-1);
}

void CYUVPlayerDlg::close_image(int8 s8DlgIdx)
{
	int32	s32ImgIdx;

	
	SetFocus();

	if (s8DlgIdx == -1)
	{
		for (s32ImgIdx = 0; s32ImgIdx < s8ImgNum; s32ImgIdx ++)
		{
			pImage[s32ImgIdx]->free_resource();
			delete(pImage[s32ImgIdx]);
			pImage[s32ImgIdx]	 = NULL;
		}

		s8ImgNum	= 0;
	}
	else
	{
		s8ImgNum --;

		if (pImage[s8DlgIdx]->bEOFFlag == TRUE)
		{
			u8EOFNum --;
		}
		if (pImage[s8DlgIdx]->bEOHFlag == TRUE)
		{
			u8EOHNum --;
		}

		pImage[s8DlgIdx]->free_resource();
		delete(pImage[s8DlgIdx]);
		pImage[s8DlgIdx]	 = NULL;

		if (s8DlgIdx != s8ImgNum)
		{
			pImage[s8ImgNum]->s8DlgIdx	= s8DlgIdx;
			pImage[s8DlgIdx]	= pImage[s8ImgNum];
		}
	}

	//++ 重排窗口位置
	for (s32ImgIdx = 0; s32ImgIdx < s8ImgNum; s32ImgIdx ++)
	{
		adjust_window_position(pImage[s32ImgIdx]);
		pImage[s32ImgIdx]->MBInfoDlg.MoveWindow((s8DlgIdx % 3) * 425, (s8DlgIdx / 3) * 16, 425, 470, FALSE);
	}

	if (s8ImgNum == 0)
	{
		if (pJumpFrameDlg != NULL)
		{
			pJumpFrameDlg->close_dlg();
		}

		initial();
	}
	else if (u8PlayMode	== COMP_MODE)
	{
		set_default_parameter_value();
		diffPic.kill_window();
		bShowDiffPic	= FALSE;
		
		if (pJumpFrameDlg != NULL)
		{
			pJumpFrameDlg->close_dlg();
			pJumpFrameDlg	= NULL;
		}
		
		m_openFile.SetFocus();
	}
}

void CYUVPlayerDlg::OnResetPic() 
{
	// TODO: Add your control notification handler code here
	for (int32 s32ImgIdx = 0; s32ImgIdx < s8ImgNum; s32ImgIdx ++)
	{
		CImageDlg	*pCurrImg	 = pImage[s32ImgIdx];

		pCurrImg->s32ViewBlkX		 = -100;
		pCurrImg->s32ViewBlkY		 = -100;
		pCurrImg->s32PrevBlkX		 = -100;
		pCurrImg->s32PrevBlkY		 = -100;
		pCurrImg->s32CurrFrameNr	 = 1;
		pCurrImg->bEOFFlag			 = FALSE;
		pCurrImg->bEOHFlag			 = FALSE;
		pCurrImg->bForwardOK		 = TRUE;
		pCurrImg->bBackwardOK		 = TRUE;
		pCurrImg->MBInfoDlg.ShowWindow(SW_HIDE);
		pCurrImg->pFile->Seek(0, CFile::begin);
	}
	if (bShowDiffPic == TRUE)
	{
		diffPic.MBInfoDlg.ShowWindow(SW_HIDE);
	}

	show_one_frame(TRUE);

	if (get_play_status() != PLAY_STATUS)
	{
		m_play.SetWindowText("播    放");
		m_replay.SetWindowText("回    放");
	}

	u8EOFNum	 = 0;
	u8EOHNum	 = 0;
}

LRESULT CYUVPlayerDlg::show_one_frame(uint8 bReNotice)
{
	for (int32 s32ImgIdx = 0; s32ImgIdx < s8ImgNum; s32ImgIdx ++)
	{
		pImage[s32ImgIdx]->show_one_frame(u8ImageMode, TRUE);
	}
	
	if (bShowDiffPic == TRUE)
	{
		diffPic.show_one_frame(u8ImageMode, bReNotice);
	}
	return NULL;
}

void CYUVPlayerDlg::hide_MBinfo_dlg()
{
    for (int32 s32ImgIdx = 0; s32ImgIdx < s8ImgNum; s32ImgIdx ++)
	{
		pImage[s32ImgIdx]->MBInfoDlg.ShowWindow(SW_HIDE);
		//++ 清除图像窗口上的宏块标记
		pImage[s32ImgIdx]->s32ViewBlkX		 = -100;
		pImage[s32ImgIdx]->s32ViewBlkY		 = -100;
		pImage[s32ImgIdx]->s32PrevBlkX		 = -100;
		pImage[s32ImgIdx]->s32PrevBlkY		 = -100;
		pImage[s32ImgIdx]->Invalidate();
		pImage[s32ImgIdx]->UpdateWindow();
	}

	if (bShowDiffPic == TRUE)
	{
		diffPic.MBInfoDlg.ShowWindow(SW_HIDE);
		//++ 清除差值图像窗口上的宏块标记
		diffPic.s32ViewBlkX		 = -100;
		diffPic.s32ViewBlkY		 = -100;
		diffPic.s32PrevBlkX		 = -100;
		diffPic.s32PrevBlkY		 = -100;
		diffPic.Invalidate();
		diffPic.UpdateWindow();
	}
}

void CYUVPlayerDlg::layout_windows()
{
    for (int32 i = 0; i < s8ImgNum; i ++)
    {
        adjust_window_position(pImage[i]);
    }

    if (bShowDiffPic == TRUE)
    {
        adjust_window_position(&diffPic);
    }
}

void CYUVPlayerDlg::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	
	// TODO: Add your message handler code here
	if (bAttachFlag == TRUE)
	{
		layout_windows();
	}
}

LRESULT CYUVPlayerDlg::adjust_image(WPARAM wParam, LPARAM lParam)
{
    int32	s32Ret;
	CImageDlg	*pCurrImage		= pImage[wParam];
	CImageDlg	*pNewImg		= new CImageDlg((CWnd *)this, (CWnd *)this);
    CFile64		*pCurrFile		= new CFile64();
	
	
	if(!pCurrFile->Open(pCurrImage->pFile->GetFilePath(), CFile::shareDenyWrite)) 
	{
		delete(pCurrFile);
		AfxMessageBox("打开文件失败！", MB_ICONERROR);
		
		return NULL;
	}

	pNewImg->bEOHFlag			= pCurrImage->bEOHFlag;
	pNewImg->bEOFFlag			= pCurrImage->bEOFFlag;
	pNewImg->bForwardOK			= pCurrImage->bForwardOK;
	pNewImg->bBackwardOK		= pCurrImage->bBackwardOK;
	pNewImg->bCloseFlag			= pCurrImage->bCloseFlag;
	pNewImg->u8MirrorMode		= pCurrImage->u8MirrorMode;
	pNewImg->s16RotateAngle		= pCurrImage->s16RotateAngle;
	pNewImg->s32StartFrameNr	= pCurrImage->s32CurrFrameNr;
	pNewImg->s32CurrFrameNr		= pCurrImage->s32CurrFrameNr;
	pNewImg->frameSize			= pCurrImage->frameSize;
	pNewImg->sampleFormat		= pCurrImage->sampleFormat;
	pNewImg->zoomSize			= pCurrImage->zoomSize;
	pNewImg->pFile				= pCurrFile;
	pNewImg->s8DlgIdx			= pCurrImage->s8DlgIdx;
    pNewImg->u8SampleFormat		= pCurrImage->u8SampleFormat;
	pNewImg->s32SrcWidth		= pCurrImage->s32SrcWidth;
	pNewImg->s32SrcHeight		= pCurrImage->s32SrcHeight;
	pNewImg->s32Width			= (lParam == 0) ? pCurrImage->s32Height : pCurrImage->s32Width;
	pNewImg->s32Height			= (lParam == 0) ? pCurrImage->s32Width : pCurrImage->s32Height;
	pNewImg->s32ZoomWidth		= (lParam == 0) ? pCurrImage->s32ZoomHeight : pCurrImage->s32ZoomWidth;
	pNewImg->s32ZoomHeight		= (lParam == 0) ? pCurrImage->s32ZoomWidth : pCurrImage->s32ZoomHeight;
	pNewImg->pCriticalSection	= &(m_CriticalSection);
	pNewImg->u32LumaPicSize		= pCurrImage->u32LumaPicSize;
	pNewImg->s32MBXNum			= pCurrImage->s32MBXNum;
	pNewImg->s32MBYNum			= pCurrImage->s32MBYNum;

	pCurrImage->free_resource();
	delete(pCurrImage);

	s32Ret	 = pNewImg->create_window((CWnd *)this);
	if (s32Ret == FAILED_YUVPlayer)
	{
		if (pCurrFile->m_hFile != pCurrFile->hFileNull)
		{
			pCurrFile->Close();
		}

		delete(pNewImg);
	}
	else
	{
		pNewImg->ShowWindow(SW_SHOW);
		pImage[wParam]	 = pNewImg;
	}

	layout_windows();
	return NULL;
}

LRESULT CYUVPlayerDlg::deal_stop_apply(WPARAM wParam, LPARAM lParam)
{
	if (bReversePlay == TRUE)
	{
 		m_replay.SetFocus();
		m_replay.SetWindowText("继续回放");
	}
	else
	{
 		m_play.SetFocus();
		m_play.SetWindowText("继续播放");
	}

	switch(s8StopValue)
	{
	case PAUSE_VALUE_0:
		exit_program();
		break;

	case PAUSE_VALUE_1:
		forward_one_step();
		break;

	case PAUSE_VALUE_2:
		forward_multistep();
	    break;

	case PAUSE_VALUE_3:
		backward_one_step();
	    break;

	case PAUSE_VALUE_4:
		backward_multistep();
		break;

	case PAUSE_VALUE_5:
		close_image(s8PlusValue);
		break;

	case PAUSE_VALUE_6:
		close_all_image();
	    break;

	case PAUSE_VALUE_7:
		set_view_mode();
	    break;

	case PAUSE_VALUE_8:
		set_comp_mode();
		break;
		
	case PAUSE_VALUE_9:
		open_files();
		break;

	case PAUSE_VALUE_10:
		diffPic.button_down_left();
	    break;

	case PAUSE_VALUE_11:
		diffPic.button_down_right();
	    break;

	case PAUSE_VALUE_12:
		pImage[s8PlusValue]->button_down_left();
		break;

	case PAUSE_VALUE_13:
		pImage[s8PlusValue]->button_down_right();
		break;

	case PAUSE_VALUE_14:
		m_frameRate.SetFocus();
		change_frame_rate();
		break;

	default:
//		AfxMessageBox("未定义停止播放请求！");
	    break;
	}

	s8StopValue		= VALUE_INVALID;
	return NULL;//For compat updating from vc6.0 to vs2008.
}

LRESULT CYUVPlayerDlg::translate_message(WPARAM wParam, LPARAM lParam)
{
    MSG cMsg;


    cMsg.message    = wParam;
    cMsg.wParam     = lParam;
    PreTranslateMessage(&cMsg);
	return NULL;
}

BOOL CYUVPlayerDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN)  //++ 判断是否有按键按下
    {
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
			if (get_play_status() != STOP_STATUS)
			{
				OnClosePic();
				
				return TRUE;
			}

			break;
			
        case VK_SPACE:
            if (get_play_status() != STOP_STATUS)
            {
                if (bReversePlay == TRUE)
                {
                    OnReplay();
                }
                else
                {
                    OnPlay();
                }
            }
            
            return TRUE;
            break;
			
		case VK_HOME:
			if (get_play_status() != STOP_STATUS)
			{
				OnResetPic();
				
				return TRUE;
			}

			break;
			
		case VK_PRIOR:
			if (get_play_status() != STOP_STATUS)
			{
				OnBackwardMultistep();
				
				return TRUE;
			}

			break;
			
		case VK_NEXT:
			if (get_play_status() != STOP_STATUS)
			{
				OnForwardMultistep();
				
				return TRUE;
			}

			break;
			
		case VK_UP:
			if (get_play_status() != STOP_STATUS)
			{
				OnBackwardOneStep();
				
				return TRUE;
			}

			break;
			
		case VK_DOWN:
			if (get_play_status() != STOP_STATUS)
			{
				OnForwardOneStep();
				
				return TRUE;
			}

			break;

		case VK_R:
			if (get_play_status() != STOP_STATUS)
			{
				bReversePlay	 = !bReversePlay;
				if (bReversePlay == TRUE)
				{
					m_replay.SetFocus();
					if (get_play_status() == PLAY_STATUS)
					{
						m_replay.SetWindowText("暂    停");
						m_play.SetWindowText("播    放");
					}
				}
				else
				{
					m_play.SetFocus();
					if (get_play_status() == PLAY_STATUS)
					{
						m_play.SetWindowText("暂    停");
						m_replay.SetWindowText("回    放");
					}
				}
			}

            return TRUE;
			break;

		case VK_RETURN:
		case VK_DELETE:
		case VK_DECIMAL:
		case VK_BACK:
		case VK_TAB:
		case VK_LEFT:
		case VK_RIGHT:

			break;
			
		default:
			if ((pMsg->wParam >= VK_A) && (pMsg->wParam <= VK_Z))
			{
				if (s8ImgNum != 0)
				{
					bEnMBInfo	 = !bEnMBInfo;

					if (bEnMBInfo == FALSE)
					{
						hide_MBinfo_dlg();
					}
				}

				return TRUE;
			}
            else if (((pMsg->wParam < VK_0) || (pMsg->wParam > VK_9)) && ((pMsg->wParam < VK_NUMPAD0) || (pMsg->wParam > VK_NUMPAD9)))
            {
                return TRUE;
            }

			break;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CYUVPlayerDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER);
	
	CDialog::OnLButtonUp(nFlags, point);
}

void CYUVPlayerDlg::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER);
	
	CDialog::OnRButtonUp(nFlags, point);
}
