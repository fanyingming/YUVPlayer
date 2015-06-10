// DiffPicDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DiffPicDlg.h"
#include "YUVPlayerDlg.h"
#include "log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDiffPicDlg dialog


CDiffPicDlg::CDiffPicDlg(CWnd* pParent /*=NULL*/)
	: CChildDlg(CDiffPicDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDiffPicDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CDiffPicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiffPicDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDiffPicDlg, CDialog)
	//{{AFX_MSG_MAP(CDiffPicDlg)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_MENUITEM_JUMP, OnMenuitemJump)
	ON_COMMAND(ID_MENUITEM_YUV, OnMenuitemYUV)
	ON_COMMAND(ID_MENUITEM_Y, OnMenuitemY)
	ON_COMMAND(ID_MENUITEM_U, OnMenuitemU)
	ON_COMMAND(ID_MENUITEM_V, OnMenuitemV)
	ON_COMMAND(ID_MENUITEM_BIN_MODE, OnMenuitemBinMode)
	ON_COMMAND(ID_MENUITEM_ABS_MODE, OnMenuitemAbsMode)
	ON_COMMAND(ID_MENUITEM_SHOWMBINFO, OnMenuitemShowmbinfo)
	ON_COMMAND(ID_MENUITEM_NOTICE, OnMenuitemNotice)
	ON_COMMAND(ID_MENUITEM_ATTACH, OnMenuitemAttach)
	ON_WM_SIZING()
	ON_WM_MOVING()
	ON_MESSAGE(WM_ENTERSIZEMOVE, OnEnterSizeMove)
	ON_MESSAGE(WM_EXITSIZEMOVE, OnExitSizeMove)
	ON_MESSAGE(WM_MYMESSAGE_2, show_one_frame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiffPicDlg message handlers
int32 CDiffPicDlg::initial()
{
    int32   s32Ret;

	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;
	CImageDlg	    *pImage0	 = pMainDlg->pImage[0];
	CImageDlg	    *pImage1	 = pMainDlg->pImage[1];


	u8DiffMode			= BIN_MODE;
	s8DlgIdx			= 2;   //++ 两图对比只可能有两幅图像，该值为 2 才能复用 adjust_window_position 函数
    u8SampleFormat		= pImage0->u8SampleFormat;
	s32Width			= pImage0->s32SrcWidth;
	s32Height			= pImage0->s32SrcHeight;
	s32ZoomWidth		= (pImage0->s32ZoomWidth != pImage1->s32ZoomWidth) ? s32Width : pImage0->s32ZoomWidth;
	s32ZoomHeight		= (pImage0->s32ZoomHeight != pImage1->s32ZoomHeight) ? s32Height : pImage0->s32ZoomHeight;
	pCriticalSection	= pImage0->pCriticalSection;
	u32LumaPicSize		= pImage0->u32LumaPicSize;
	u32ChroPicSize		= pImage0->u32ChroPicSize;
    u32FrameSize		= pImage0->u32FrameSize;
	s32MBXNum			= pImage0->s32MBXNum;
	s32MBYNum			= pImage0->s32MBYNum;

    pSubMenu->CheckMenuItem(ID_MENUITEM_BIN_MODE, MF_BYCOMMAND | MF_CHECKED);
    pSubMenu->CheckMenuItem(ID_MENUITEM_ABS_MODE, MF_BYCOMMAND | MF_UNCHECKED);
	pSubMenu->EnableMenuItem(ID_MENUITEM_MIRROR_HORI, MF_BYCOMMAND | MF_DISABLED);
	pSubMenu->EnableMenuItem(ID_MENUITEM_MIRROR_VERT, MF_BYCOMMAND | MF_DISABLED);
	pSubMenu->EnableMenuItem(ID_MENUITEM_ROTATE_90, MF_BYCOMMAND | MF_DISABLED);
	pSubMenu->EnableMenuItem(ID_MENUITEM_ROTATE_270, MF_BYCOMMAND | MF_DISABLED);

	s32Ret	 = malloc_memory();
	if (s32Ret == FAILED_YUVPlayer)
	{
		return FAILED_YUVPlayer;
	}

	pOrigYUV[0]			= pReadYUV[0];
	pOrigYUV[1]			= pReadYUV[1];
	pOrigYUV[2]			= pReadYUV[2];

	set_bmp_parameter();

	//++ 创建字体
	VERIFY(font.CreateFont(13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "宋体"));

	return SUCCEEDED_YUVPlayer;
}

int32 CDiffPicDlg::create_window(CWnd *pMainDlg)
{
	int32	s32Ret;


#ifdef LOGINFO
	CLog("创建差值窗口");
#endif
	this->pMainDlg	 = pMainDlg;
	fileName	 = "差值图像";
	Create(IDD_DIFFPIC_DIALOG, pMainDlg);
	s32Ret	= initial();
	if (s32Ret == FAILED_YUVPlayer)
	{
		return FAILED_YUVPlayer;
	}
	
	//++ 创建宏块信息窗口
	MBInfoDlg.create_window((CWnd *)this);
	MBInfoDlg.MoveWindow(2 * 425, 0, 425, 470, FALSE);

	return SUCCEEDED_YUVPlayer;
}

void CDiffPicDlg::kill_window()
{
	free(pYUVBuff);
	GlobalFree(hloc);
	
	MBInfoDlg.talbeFont.DeleteObject();
	MBInfoDlg.DestroyWindow();
	font.DeleteObject();
	DestroyWindow();

	m_hWnd	= NULL;
}

void CDiffPicDlg::show_window()
{
	CRect   currRect;
	CRect	prevRect;
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;
	CImageDlg	    *pPrevImg	 = (CImageDlg *)(pMainDlg->pImage[1]);


#ifdef LOGINFO
	CLog("显示差值窗口");
#endif
	//++ 设置图像窗口大小及位置
	resize_window();

	GetWindowRect(currRect);
	pPrevImg->GetWindowRect(prevRect);
	pMainDlg->bShowDiffPic	= TRUE;
    pMainDlg->adjust_window_position(this);
	
#ifdef LOGINFO
	CLog("读入首帧差值图像");
#endif
	//++ 读入首帧差值图像
	get_one_frame(pMainDlg->u8ImageMode);
	ShowWindow(SW_SHOW);
	
	if ((bSameFlag == FALSE) && (pMainDlg->bNoticeFlag == TRUE))
	{
		int32	s32Ret;
		
		s32Ret	= AfxMessageBox("当前两幅图像不相同！是否继续通知？", MB_YESNO | MB_ICONINFORMATION);
		if (s32Ret == IDNO)
		{
			pMainDlg->bNoticeFlag	= FALSE;
		}
	}
}

void CDiffPicDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	pMainDlg->set_view_mode();
}

CString CDiffPicDlg::get_title() 
{
	CString	sTitle;
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	switch(pMainDlg->u8ImageMode)
	{
	case IMAGE_YUV:
		sTitle	 = "(YUV";
		break;

	case IMAGE_Y:
		sTitle	 = "( Y ";
		break;

	case IMAGE_U:
		sTitle	 = "( U ";
		break;

	case IMAGE_V:
		sTitle	 = "( V ";
		break;

	default:
		break;
	}

	switch(u8DiffMode)
	{
	case BIN_MODE:
		sTitle	+= "/二值化)";
		break;

	case ABS_MODE:
		sTitle	+= "/绝对值)";
		break;

	default:
		break;
	}

	return sTitle;
}

void CDiffPicDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	
	
	SetWindowText(fileName + get_title());
    //++ 显示图像
    show_image(&dc);
    //++ 防止窗口被遮挡后，窗口上标记的内容丢失
	remark_macroblock(&dc);

	if (bSameFlag == TRUE)
	{
		CRect	textRect(5, 5, 10, 10);

		dc.SelectObject(&font);
		dc.DrawText("相同", &textRect, DT_LEFT | DT_VCENTER | DT_NOCLIP);
	}
    // Do not call CFrameWnd::OnPaint() for painting messages

    GetWindowRect(&wndRect);
    oldSizeRect	= wndRect;
}

void CDiffPicDlg::get_one_frame(uint8 u8ImageMode)
{
	int32	i;
	int32	j;
	uint32	u32Offset;
	uint8	**pYUV1;
	uint8	**pYUV2;
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	bSameFlag	= TRUE;
	u32Offset	= 0;
	pYUV1	 = pMainDlg->pImage[0]->pReadYUV;
	pYUV2	 = pMainDlg->pImage[1]->pReadYUV;

	switch (u8SampleFormat)
	{
	case YUV420:
		u32Offset	 = 0;
		for (j = 0; j < (s32Height >> 1); j++)
		{
			for (i = 0; i< (s32Width >> 1); i ++)
			{
				int32	s32Diff	 = pYUV2[1][u32Offset] - pYUV1[1][u32Offset];
				
				pOrigYUV[1][u32Offset]	 = u8DiffMode ? ABS(s32Diff) : BIN(s32Diff);
				bSameFlag	&= (!s32Diff);
				u32Offset ++;
			}
		}
		
		u32Offset	 = 0;
		for (j = 0; j < (s32Height >> 1); j++)
		{
			for (i = 0; i< (s32Width >> 1); i ++)
			{
				int32	s32Diff	 = pYUV2[2][u32Offset] - pYUV1[2][u32Offset];
				
				pOrigYUV[2][u32Offset]	 = u8DiffMode ? ABS(s32Diff) : BIN(s32Diff);
				bSameFlag	&= (!s32Diff);
				u32Offset ++;
			}
		}
	case YUV400:
		u32Offset	 = 0;
		for (j = 0; j < s32Height; j++)
		{
			for (i = 0; i< s32Width; i ++)
			{
				int32	s32Diff	 = pYUV2[0][u32Offset] - pYUV1[0][u32Offset];
				
				pOrigYUV[0][u32Offset]	 = u8DiffMode ? ABS(s32Diff) : BIN(s32Diff);
				bSameFlag	&= (!s32Diff);
				u32Offset ++;
			}
		}
		
		break;

	default:
		break;
	}

	color_space_convert(u8ImageMode);
}

LRESULT CDiffPicDlg::show_one_frame(WPARAM wParam, LPARAM lParam)
{
	uint8	u8ImageMode	 = (uint8)wParam;
	uint8	bReNotice	 = (uint8)lParam;
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	get_one_frame(u8ImageMode);
	InvalidateRect(NULL, FALSE);
	UpdateWindow();
	if ((bReNotice == TRUE) && (bSameFlag == FALSE) && (pMainDlg->bNoticeFlag == TRUE))
	{
		int32	s32Ret;
		
		s32Ret	= AfxMessageBox("当前两幅图像不相同！是否继续通知？", MB_YESNO | MB_ICONINFORMATION);
		if (s32Ret == IDNO)
		{
			pMainDlg->bNoticeFlag	= FALSE;
		}
		
		pMainDlg->set_play_status(PAUSE_STATUS);
	}
	return NULL;
}

void CDiffPicDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	if (pMainDlg->get_play_status() != PLAY_STATUS)
	{
		int32	s32SrcX;	//++ 鼠标所在点对应未缩放图像中的点的坐标
		int32	s32SrcY;	//++ 鼠标所在点对应未缩放图像中的点的坐标
		
		s32SrcX		= point.x * s32Width / s32ZoomWidth;
		s32SrcY		= point.y * s32Height / s32ZoomHeight;
		s32MBXIdx	= s32SrcX >> 4;
		s32MBYIdx	= s32SrcY >> 4;
		//++ 标记鼠标所指的宏块
		mark_macroblock();
		pMainDlg->pImage[0]->s32MBXIdx		= s32MBXIdx;
		pMainDlg->pImage[0]->s32MBYIdx		= s32MBYIdx;
		pMainDlg->pImage[0]->mark_macroblock();
		pMainDlg->pImage[1]->s32MBXIdx		= s32MBXIdx;
		pMainDlg->pImage[1]->s32MBYIdx		= s32MBYIdx;
		pMainDlg->pImage[1]->mark_macroblock();
	}
	
	CDialog::OnMouseMove(nFlags, point);
}

void CDiffPicDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	s32ViewMBx	= s32CurrMBx;
	s32ViewMBy	= s32CurrMBy;
	pMainDlg->pImage[0]->s32ViewMBx		= s32CurrMBx;
	pMainDlg->pImage[0]->s32ViewMBy		= s32CurrMBy;
	pMainDlg->pImage[1]->s32ViewMBx		= s32CurrMBx;
	pMainDlg->pImage[1]->s32ViewMBy		= s32CurrMBy;
	
	if (pMainDlg->get_play_status() != PLAY_STATUS)
	{
		button_down_left();
	}
	else
	{
		pMainDlg->s8StopValue	 = PAUSE_VALUE_10;
		pMainDlg->set_play_status(PAUSE_STATUS);
	}
}

void CDiffPicDlg::button_down_left()
{
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	//++ 显示宏块信息
	pMainDlg->bShowMBInfo	= TRUE;
	pMainDlg->pImage[0]->view_macroblock();
	pMainDlg->pImage[1]->view_macroblock();
	view_macroblock();

//	CFrameWnd::OnLButtonDown(nFlags, point);
}

void CDiffPicDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int32	s32SrcX;	//++ 鼠标所在点对应未缩放图像中的点的坐标
	int32	s32SrcY;	//++ 鼠标所在点对应未缩放图像中的点的坐标
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;

	
	s32SrcX		= point.x * s32Width / s32ZoomWidth;
	s32SrcY		= point.y * s32Height / s32ZoomHeight;
	s32MBXIdx	= s32SrcX >> 4;
	s32MBYIdx	= s32SrcY >> 4;
	pMainDlg->pImage[0]->s32MBXIdx		= s32MBXIdx;
	pMainDlg->pImage[0]->s32MBYIdx		= s32MBYIdx;
	pMainDlg->pImage[1]->s32MBXIdx		= s32MBXIdx;
	pMainDlg->pImage[1]->s32MBYIdx		= s32MBYIdx;

	if (pMainDlg->get_play_status() != PLAY_STATUS)
	{
		button_down_right();
	}
	else
	{
		pMainDlg->s8StopValue	 = PAUSE_VALUE_11;
		pMainDlg->set_play_status(PAUSE_STATUS);
	}
}

void CDiffPicDlg::button_down_right()
{
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	//++ 标记鼠标所指的宏块
	mark_macroblock();
	pMainDlg->pImage[0]->mark_macroblock();
	pMainDlg->pImage[1]->mark_macroblock();
	//++ 载入右键菜单
	show_mouse_menu();
	
//	CFrameWnd::OnRButtonDown(nFlags, point);
}

void CDiffPicDlg::OnMenuitemJump() 
{
	// TODO: Add your command handler code here
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	if (pMainDlg->pJumpFrameDlg == NULL)
	{
		//++ 创建自定义窗口
		CJumpFrameDlg	jumpFrameDlg((CWnd *)this, (CWnd *)pMainDlg);
		
		pMainDlg->pJumpFrameDlg		= &jumpFrameDlg;
		pMainDlg->pJumpFrameDlg->bDiffDlgFlag	= TRUE;
		//++ 对比模式时跳帧窗口快捷键需要用到 pJumpImg
		if (pMainDlg->pImage[0]->s32FrameNum < pMainDlg->pImage[1]->s32FrameNum)
		{
			pMainDlg->pJumpFrameDlg->s32MaxFrameNum		= pMainDlg->pImage[0]->s32FrameNum;
		}
		else
		{
			pMainDlg->pJumpFrameDlg->s32MaxFrameNum		= pMainDlg->pImage[1]->s32FrameNum;
		}
		pMainDlg->pJumpFrameDlg->DoModal();
		pMainDlg->pJumpFrameDlg		= NULL;
	}
	else
	{
		pMainDlg->pJumpFrameDlg->close_dlg();
		//++ 创建自定义窗口
		CJumpFrameDlg	jumpFrameDlg((CWnd *)this, (CWnd *)pMainDlg);
		
		pMainDlg->pJumpFrameDlg		= &jumpFrameDlg;
		pMainDlg->pJumpFrameDlg->bDiffDlgFlag	= TRUE;
		//++ 对比模式时跳帧窗口快捷键需要用到 pJumpImg
		if (pMainDlg->pImage[0]->s32FrameNum < pMainDlg->pImage[1]->s32FrameNum)
		{
			pMainDlg->pJumpFrameDlg->s32MaxFrameNum		= pMainDlg->pImage[0]->s32FrameNum;
		}
		else
		{
			pMainDlg->pJumpFrameDlg->s32MaxFrameNum		= pMainDlg->pImage[1]->s32FrameNum;
		}
		pMainDlg->pJumpFrameDlg->DoModal();
		pMainDlg->pJumpFrameDlg		= NULL;
	}
}

void CDiffPicDlg::OnMenuitemYUV() 
{
	// TODO: Add your command handler code here
    set_image_mode(IMAGE_YUV);
}

void CDiffPicDlg::OnMenuitemY() 
{
	// TODO: Add your command handler code here
	set_image_mode(IMAGE_Y);
}

void CDiffPicDlg::OnMenuitemU() 
{
	// TODO: Add your command handler code here
    set_image_mode(IMAGE_U);
}

void CDiffPicDlg::OnMenuitemV() 
{
	// TODO: Add your command handler code here
    set_image_mode(IMAGE_V);
}

void CDiffPicDlg::OnMenuitemShowmbinfo() 
{
	// TODO: Add your command handler code here
    enable_mbinfo_dlg();
}

void CDiffPicDlg::OnMenuitemNotice() 
{
	// TODO: Add your command handler code here
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	pMainDlg->bNoticeFlag	= !pMainDlg->bNoticeFlag;
}

void CDiffPicDlg::OnMenuitemBinMode() 
{
	// TODO: Add your command handler code here
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	u8DiffMode	 = BIN_MODE;
	if (pMainDlg->get_play_status() != PLAY_STATUS)
	{
		show_one_frame(pMainDlg->u8ImageMode, FALSE);
		show_macroblock_info();
	}

    pSubMenu->CheckMenuItem(ID_MENUITEM_BIN_MODE, MF_BYCOMMAND | MF_CHECKED);
    pSubMenu->CheckMenuItem(ID_MENUITEM_ABS_MODE, MF_BYCOMMAND | MF_UNCHECKED);
}

void CDiffPicDlg::OnMenuitemAbsMode() 
{
	// TODO: Add your command handler code here
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	u8DiffMode	 = ABS_MODE;
	if (pMainDlg->get_play_status() != PLAY_STATUS)
	{
		show_one_frame(pMainDlg->u8ImageMode, FALSE);
		show_macroblock_info();
	}

    pSubMenu->CheckMenuItem(ID_MENUITEM_ABS_MODE, MF_BYCOMMAND | MF_CHECKED);
    pSubMenu->CheckMenuItem(ID_MENUITEM_BIN_MODE, MF_BYCOMMAND | MF_UNCHECKED);
}

void CDiffPicDlg::OnMenuitemAttach() 
{
	// TODO: Add your command handler code here
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	pMainDlg->bAttachFlag	= !pMainDlg->bAttachFlag;
}

void CDiffPicDlg::OnSizing(UINT fwSide, LPRECT pRect) 
{
	CDialog::OnSizing(fwSide, pRect);
	
	// TODO: Add your message handler code here
	bSizeChanged	= TRUE;
	change_size(pRect);
}

LRESULT CDiffPicDlg::OnEnterSizeMove(WPARAM wp, LPARAM lp)
{
    GetWindowRect(&wndRect);
    oldSizeRect		= wndRect;
	
	return TRUE;
}

LRESULT CDiffPicDlg::OnExitSizeMove(WPARAM wp, LPARAM lp)
{
	if (bSizeChanged == TRUE)
	{
		update_image();
		
		bSizeChanged	= FALSE;
	}
	
	return TRUE;
}

void CDiffPicDlg::OnMoving(UINT fwSide, LPRECT pRect) 
{
	CDialog::OnMoving(fwSide, pRect);
	
	// TODO: Add your message handler code here
	change_location(pRect);
}

BOOL CDiffPicDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
    CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;
    
    
    if(pMsg->message == WM_KEYDOWN)  //++ 判断是否有按键按下
    {
        ::PostMessage(pMainDlg->m_hWnd, WM_MYMESSAGE_3, pMsg->message, pMsg->wParam);
        
        return TRUE;
    }
    
    return CDialog::PreTranslateMessage(pMsg);
}
