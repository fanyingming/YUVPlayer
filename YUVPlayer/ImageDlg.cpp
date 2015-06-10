// ImageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageDlg.h"
#include "YUVPlayerDlg.h"
#include "multithread.h"
#include "log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageDlg dialog


CImageDlg::CImageDlg(CWnd* pParent /*=NULL*/)
	: CChildDlg(CImageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImageDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CImageDlg::CImageDlg(CWnd* pParent, CWnd* pMainDlg)
	: CChildDlg(CImageDlg::IDD, pParent)
{
	this->pMainDlg		= pMainDlg;
}

void CImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImageDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImageDlg, CDialog)
	//{{AFX_MSG_MAP(CImageDlg)
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
	ON_COMMAND(ID_MENUITEM_GOSAMEFRAME, OnMenuitemGosameframe)
	ON_COMMAND(ID_MENUITEM_SHOWMBINFO, OnMenuitemShowmbinfo)
	ON_COMMAND(ID_MENUITEM_NOTICE, OnMenuitemNotice)
	ON_COMMAND(ID_MENUITEM_ATTACH, OnMenuitemAttach)
	ON_WM_SIZING()
	ON_WM_MOVING()
	ON_MESSAGE(WM_ENTERSIZEMOVE, OnEnterSizeMove)
	ON_MESSAGE(WM_EXITSIZEMOVE, OnExitSizeMove)
	ON_COMMAND(ID_MENUITEM_MIRROR_HORI, OnMenuitemMirrorHori)
	ON_COMMAND(ID_MENUITEM_MIRROR_VERT, OnMenuitemMirrorVert)
	ON_MESSAGE(WM_MYMESSAGE_1, show_one_frame)
	ON_COMMAND(ID_MENUITEM_ROTATE_90, OnMenuitemRotate90)
	ON_COMMAND(ID_MENUITEM_ROTATE_270, OnMenuitemRotate270)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageDlg message handlers

int32 CImageDlg::create_window(CWnd *pMainDlg)
{
    int32   s32Ret;


    fileName	 = pFile->GetFileTitle();
	Create(IDD_IMAGE_DIALOG, pMainDlg);
	
	s32Ret	 = initial();
	if (s32Ret == FAILED_YUVPlayer)
	{
		return FAILED_YUVPlayer;
	}

#ifdef LOGINFO
	CLog("创建图像窗口>>创建宏块信息窗口");
#endif
	//++ 创建宏块信息窗口
	MBInfoDlg.create_window((CWnd *)this);
	MBInfoDlg.MoveWindow((s8DlgIdx % 3) * 425, (s8DlgIdx / 3) * 16, 425, 470, FALSE);

#ifdef LOGINFO
	CLog("创建图像窗口>>创建跳转帧号窗口");
#endif
    //++ 初始化右键菜单
    pSubMenu->EnableMenuItem(ID_MENUITEM_BIN_MODE, MF_BYCOMMAND | MF_GRAYED);
    pSubMenu->EnableMenuItem(ID_MENUITEM_ABS_MODE, MF_BYCOMMAND | MF_GRAYED);
    pSubMenu->CheckMenuItem(ID_MENUITEM_BIN_MODE, MF_BYCOMMAND | MF_UNCHECKED);

	return SUCCEEDED_YUVPlayer;
}

int32 CImageDlg::initial()
{
	int32	s32Ret;
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


    switch (u8SampleFormat)
    {
    case YUV400:
        u32ChroPicSize	 = (s32Width >> 1) * (s32Height >> 1);  //++ 按 4:2:0 计算色度空间
        u32FrameSize	 = u32LumaPicSize;
        s32FrameNum		 = pFile->GetLength() / u32FrameSize;
        
        break;
        
    case YUV420:
        u32ChroPicSize	 = (s32Width >> 1) * (s32Height >> 1);
        u32FrameSize	 = u32LumaPicSize + (u32ChroPicSize << 1);
        s32FrameNum		 = pFile->GetLength() / u32FrameSize;
        
        break;
        
    default:
        
        break;
    }

	if (s32FrameNum == 0)
	{
		AfxMessageBox(fileName + " 不够一帧数据！", MB_ICONERROR);

		return FAILED_YUVPlayer;
	}

	if (s32StartFrameNr > s32FrameNum)
	{
		CString	message;
		
		message.Format(" 只有 %d 帧，起始播放帧号无效！", s32FrameNum);
		AfxMessageBox(fileName + message, MB_ICONERROR);

		return FAILED_YUVPlayer;
	}

#ifdef LOGINFO
	CLog("创建图像窗口>>分配内存");
#endif
	//++ 分配内存
	s32Ret  = malloc_memory();
	if (s32Ret == FAILED_YUVPlayer)
	{
		return FAILED_YUVPlayer;
	}

	set_bmp_parameter();

#ifdef LOGINFO
	CLog("创建图像窗口>>读入首帧图像");
#endif
	//++ 读入首帧图像
    s32Ret  = jump_to_frame(pMainDlg->u8ImageMode, s32StartFrameNr);
	if (s32Ret == EOF_YUVPlayer)
	{
		bEOFFlag		= TRUE;
		bBackwardOK		= FALSE;

		free(pYUVBuff);
		GlobalFree(hloc);

		AfxMessageBox(fileName + " 读入数据错误！", MB_ICONERROR);

		return FAILED_YUVPlayer;
	}

#ifdef LOGINFO
	CLog("创建图像窗口>>设置窗口大小");
#endif
	//++ 设置图像窗口大小及位置
	resize_window();
#ifdef LOGINFO
	CLog("创建图像窗口>>设置窗口位置");
#endif
	pMainDlg->adjust_window_position(this);

	return SUCCEEDED_YUVPlayer;
}

void CImageDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CString	sTitle;
	// TODO: Add your message handler code here


	sTitle.Format("%d/%d - %s", s32CurrFrameNr, s32FrameNum, fileName);
    SetWindowText(sTitle);
    //++ 显示图像
    show_image(&dc);
	//++ 防止窗口被遮挡后，窗口上标记的内容丢失
	remark_macroblock(&dc);
	// Do not call CFrameWnd::OnPaint() for painting messages
}

void CImageDlg::rotate_image(LPBYTE pSrcY, LPBYTE pSrcU, LPBYTE pSrcV)
{
	int32	i, j;
	uint8*	pu8SrcY;
	uint8*	pu8SrcU;
	uint8*	pu8SrcV;
	uint8*	pu8DstY;
	uint8*	pu8DstU;
	uint8*	pu8DstV;


	if (s16RotateAngle == 0)
	{
		pOrigYUV[0]	= pSrcY;
		pOrigYUV[1]	= pSrcU;
		pOrigYUV[2]	= pSrcV;
		
		return;
	}
	
	if (s16RotateAngle == 90)
	{
		switch (u8SampleFormat)
		{
		case YUV420:
			pu8SrcU	= pSrcU + (s32SrcWidth >> 1);
			pu8SrcV	= pSrcV + (s32SrcWidth >> 1);
			for (j = 0; j < (s32SrcHeight >> 1); j ++)
			{
				pu8DstU	= pRotaYUV[1] + u32ChroPicSize - j;
				pu8DstV	= pRotaYUV[2] + u32ChroPicSize - j;
				for (i = 0; i < (s32SrcWidth >> 1); i ++)
				{
					pu8DstU[0]	 = pu8SrcU[-i];
					pu8DstV[0]	 = pu8SrcV[-i];
					pu8DstU	-= (s32SrcHeight >> 1);
					pu8DstV	-= (s32SrcHeight >> 1);
				}
				
				pu8SrcU	+= (s32SrcWidth >> 1);
				pu8SrcV	+= (s32SrcWidth >> 1);
			}
		case YUV400:
			pu8SrcY	= pSrcY + s32SrcWidth - 1;		
			for (j = 0; j < s32SrcHeight; j ++)
			{
				pu8DstY	= pRotaYUV[0] + u32LumaPicSize - j - 1;
				
				for (i = 0; i < s32SrcWidth; i ++)
				{
					pu8DstY[0]	 = pu8SrcY[-i];
					pu8DstY		-= s32SrcHeight;
				}
				
				pu8SrcY	+= s32SrcWidth;		
			}
			
			break;
		default:
			break;
		}
	}
	else if (s16RotateAngle == 180)
	{
		switch (u8SampleFormat)
		{
		case YUV420:
			pu8SrcU	= pSrcU;
			pu8SrcV	= pSrcV;
			pu8DstU	= pRotaYUV[1] + u32ChroPicSize - 1;
			pu8DstV	= pRotaYUV[2] + u32ChroPicSize - 1;
			for (j = 0; j < (s32SrcHeight >> 1); j ++)
			{
				for (i = 0; i < (s32SrcWidth >> 1); i ++)
				{
					pu8DstU[-i]	 = pu8SrcU[i];
					pu8DstV[-i]	 = pu8SrcV[i];
				}
				
				pu8SrcU	+= (s32SrcWidth >> 1);
				pu8SrcV	+= (s32SrcWidth >> 1);
				pu8DstU	-= (s32SrcWidth >> 1);
				pu8DstV	-= (s32SrcWidth >> 1);
			}
		case YUV400:
			pu8SrcY	= pSrcY;		
			pu8DstY	= pRotaYUV[0] + u32LumaPicSize - 1;
			for (j = 0; j < s32SrcHeight; j ++)
			{
				for (i = 0; i < s32SrcWidth; i ++)
				{
					pu8DstY[-i]	 = pu8SrcY[i];
				}
				
				pu8SrcY	+= s32SrcWidth;		
				pu8DstY	-= s32SrcWidth;
			}
			break;
		default:
			break;
		}
	}
	else if (s16RotateAngle == 270)
	{
		switch (u8SampleFormat)
		{
		case YUV420:
			pu8SrcU	= pSrcU;
			pu8SrcV	= pSrcV;
			for (j = 0; j < (s32SrcHeight >> 1); j ++)
			{
				pu8DstU	= pRotaYUV[1] + u32ChroPicSize - (s32SrcHeight >> 1) + j;
				pu8DstV	= pRotaYUV[2] + u32ChroPicSize - (s32SrcHeight >> 1) + j;
				for (i = 0; i < (s32SrcWidth >> 1); i ++)
				{
					pu8DstU[0]	 = pu8SrcU[i];
					pu8DstV[0]	 = pu8SrcV[i];
					pu8DstU	-= (s32SrcHeight >> 1);
					pu8DstV	-= (s32SrcHeight >> 1);
				}
				
				pu8SrcU	+= (s32SrcWidth >> 1);
				pu8SrcV	+= (s32SrcWidth >> 1);
			}
		case YUV400:
			pu8SrcY	= pSrcY;		
			for (j = 0; j < s32SrcHeight; j ++)
			{
				pu8DstY	= pRotaYUV[0] + u32LumaPicSize - s32SrcHeight + j;
				
				for (i = 0; i < s32SrcWidth; i ++)
				{
					pu8DstY[0]	 = pu8SrcY[i];
					pu8DstY		-= s32SrcHeight;
				}
				
				pu8SrcY	+= s32SrcWidth;		
			}
			
			break;
		default:
			break;
		}
	}
	
	pOrigYUV[0]	= pRotaYUV[0];
	pOrigYUV[1]	= pRotaYUV[1];
	pOrigYUV[2]	= pRotaYUV[2];
}

void CImageDlg::mirror_image(LPBYTE pSrcY, LPBYTE pSrcU, LPBYTE pSrcV)
{
	int32	i, j;
	uint8*	pu8SrcY;
	uint8*	pu8SrcU;
	uint8*	pu8SrcV;
	uint8*	pu8DstY;
	uint8*	pu8DstU;
	uint8*	pu8DstV;


	if (u8MirrorMode == MIRROR_NONE)
	{
		pOrigYUV[0]	= pSrcY;
		pOrigYUV[1]	= pSrcU;
		pOrigYUV[2]	= pSrcV;

		return;
	}

	if (u8MirrorMode == MIRROR_BOTH)
	{
		switch (u8SampleFormat)
		{
		case YUV420:
			for (i = 0; i < u32ChroPicSize; i ++)
			{
				pMirrYUV[1][i]	 = pSrcU[u32ChroPicSize - 1 - i];
				pMirrYUV[2][i]	 = pSrcV[u32ChroPicSize - 1 - i];
			}
		case YUV400:
			for (i = 0; i < u32LumaPicSize; i ++)
			{
				pMirrYUV[0][i]	 = pSrcY[u32LumaPicSize - 1 - i];
			}

			break;
		default:
			break;
		}
	}
	else if (u8MirrorMode == MIRROR_HORI)
	{
		switch (u8SampleFormat)
		{
		case YUV420:
			pu8SrcU	= pSrcU;
			pu8SrcV	= pSrcV;
			pu8DstU	= pMirrYUV[1];
			pu8DstV	= pMirrYUV[2];
			for (j = 0; j < (s32Height >> 1); j ++)
			{
				for (i = 0; i < (s32Width >> 1); i ++)
				{
					pu8DstU[i]	 = pu8SrcU[(s32Width >> 1) - 1 - i];
					pu8DstV[i]	 = pu8SrcV[(s32Width >> 1) - 1 - i];
				}
				
				pu8SrcU	+= (s32Width >> 1);
				pu8DstU	+= (s32Width >> 1);
				pu8SrcV	+= (s32Width >> 1);
				pu8DstV	+= (s32Width >> 1);
			}
		case YUV400:
			pu8SrcY	= pSrcY;		
			pu8DstY	= pMirrYUV[0];
			for (j = 0; j < s32Height; j ++)
			{
				for (i = 0; i < s32Width; i ++)
				{
					pu8DstY[i]	 = pu8SrcY[s32Width - 1 - i];
				}
				
				pu8DstY	+= s32Width;
				pu8SrcY	+= s32Width;		
			}
			
			break;
		default:
			break;
		}
	}
	else if (u8MirrorMode == MIRROR_VERT)
	{
		switch (u8SampleFormat)
		{
		case YUV420:
			pu8SrcU	= pSrcU;
			pu8SrcV	= pSrcV;
			pu8DstU	= pMirrYUV[1] + u32ChroPicSize;
			pu8DstV	= pMirrYUV[2] + u32ChroPicSize;
			for (j = 0; j < (s32Height >> 1); j ++)
			{
				pu8DstU	-= (s32Width >> 1);
				pu8DstV	-= (s32Width >> 1);
				
				for (i = 0; i < (s32Width >> 1); i ++)
				{
					memcpy(pu8DstU, pu8SrcU, (s32Width >> 1));
					memcpy(pu8DstV, pu8SrcV, (s32Width >> 1));
				}
				
				pu8SrcU	+= (s32Width >> 1);
				pu8SrcV	+= (s32Width >> 1);
			}
		case YUV400:
			pu8SrcY	= pSrcY;		
			pu8DstY	= pMirrYUV[0] + u32LumaPicSize;
			for (j = 0; j < s32Height; j ++)
			{
				pu8DstY	-= s32Width;
				
				for (i = 0; i < s32Width; i ++)
				{
					memcpy(pu8DstY, pu8SrcY, s32Width);
				}
				
				pu8SrcY	+= s32Width;		
			}
			
			break;
		default:
			break;
		}
	}

	pOrigYUV[0]	= pMirrYUV[0];
	pOrigYUV[1]	= pMirrYUV[1];
	pOrigYUV[2]	= pMirrYUV[2];
}

int32 CImageDlg::read_one_frame(uint8 u8ImageMode)
{
	//++ 启用临界区保护
	CCriticalSection	CriticalSection(pCriticalSection);
	

	switch (u8SampleFormat)
	{
	case YUV400:
		if (u32LumaPicSize != pFile->Read(pReadYUV[0], u32LumaPicSize))
		{
			return EOF_YUVPlayer;
		}
		
		break;
		
	case YUV420:
		if (u32LumaPicSize != pFile->Read(pReadYUV[0], u32LumaPicSize))
		{
			return EOF_YUVPlayer;
		}
		if (u32ChroPicSize != pFile->Read(pReadYUV[1], u32ChroPicSize))
		{
			return EOF_YUVPlayer;
		}
		if (u32ChroPicSize != pFile->Read(pReadYUV[2], u32ChroPicSize))
		{
			return EOF_YUVPlayer;
		}
		
		break;
		
	default:
		
		break;
	}

	rotate_image(pReadYUV[0], pReadYUV[1], pReadYUV[2]);
	mirror_image(pOrigYUV[0], pOrigYUV[1], pOrigYUV[2]);

	color_space_convert(u8ImageMode);

	return SUCCEEDED_YUVPlayer;
}

LRESULT CImageDlg::show_one_frame(WPARAM wParam, LPARAM lParam)
{
	uint8	u8ImageMode		= (uint8)wParam;
    int32   s32Ret;


	s32Ret  = read_one_frame(u8ImageMode);
	InvalidateRect(NULL, FALSE);
	UpdateWindow();

    return s32Ret;
}

int32 CImageDlg::jump_to_frame(uint8 u8ImageMode, int32 s32JumpFrameNr)
{
    int32   s32Ret;
    int32   s32MaxFrameNum;
	

	bEOFFlag		 = FALSE;
	bEOHFlag		 = FALSE;
	bForwardOK		 = TRUE;
	bBackwardOK		 = TRUE;
	
	pFile->Seek(0, CFile::begin);
	s32CurrFrameNr	 = s32JumpFrameNr;
    s32MaxFrameNum   = 0X7FFFFFFF / u32FrameSize;
    while (s32JumpFrameNr > s32MaxFrameNum)
    {
    	pFile->Seek(s32MaxFrameNum * u32FrameSize, CFile::current);
        s32JumpFrameNr  -= s32MaxFrameNum;
    }
	pFile->Seek((s32JumpFrameNr - 1) * u32FrameSize, CFile::current);
	s32Ret  = show_one_frame(u8ImageMode, TRUE);

    return s32Ret;
}

void CImageDlg::OnMouseMove(UINT nFlags, CPoint point) 
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
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CImageDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	s32ViewMBx	= s32CurrMBx;
	s32ViewMBy	= s32CurrMBy;

	if (pMainDlg->get_play_status() != PLAY_STATUS)
	{
		button_down_left();
	}
	else
	{
		pMainDlg->s8StopValue	 = PAUSE_VALUE_12;
		pMainDlg->s8PlusValue	 = s8DlgIdx;
		pMainDlg->set_play_status(PAUSE_STATUS);
	}
}

void CImageDlg::button_down_left()
{
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	//++ 显示宏块信息
	pMainDlg->bShowMBInfo	= TRUE;
	view_macroblock();

//	CFrameWnd::OnLButtonDown(nFlags, point);
}

void CImageDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int32	s32SrcX;	//++ 鼠标所在点对应未缩放图像中的点的坐标
	int32	s32SrcY;	//++ 鼠标所在点对应未缩放图像中的点的坐标
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;

	
	s32SrcX		= point.x * s32Width / s32ZoomWidth;
	s32SrcY		= point.y * s32Height / s32ZoomHeight;
	s32MBXIdx	= s32SrcX >> 4;
	s32MBYIdx	= s32SrcY >> 4;

	if (pMainDlg->get_play_status() != PLAY_STATUS)
	{
		button_down_right();
	}
	else
	{
		pMainDlg->s8StopValue	 = PAUSE_VALUE_13;
		pMainDlg->s8PlusValue	 = s8DlgIdx;
		pMainDlg->set_play_status(PAUSE_STATUS);
	}
}

void CImageDlg::button_down_right()
{
	//++ 标记鼠标所指的宏块
	mark_macroblock();
	//++ 载入右键菜单
	show_mouse_menu();

//	CFrameWnd::OnRButtonDown(nFlags, point);
}

void CImageDlg::OnMenuitemJump() 
{
	// TODO: Add your command handler code here
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	if (pMainDlg->pJumpFrameDlg != NULL)
	{
		pMainDlg->pJumpFrameDlg->close_dlg();
	}

	//++ 创建自定义窗口
	CJumpFrameDlg	jumpFrameDlg((CWnd *)this, (CWnd *)pMainDlg);
	
	pMainDlg->pJumpFrameDlg		= &jumpFrameDlg;
	pMainDlg->pJumpFrameDlg->bDiffDlgFlag	= FALSE;
	pMainDlg->pJumpFrameDlg->pCurrJumpImg	= this;
	pMainDlg->pJumpFrameDlg->s32MaxFrameNum		= s32FrameNum;
	pMainDlg->pJumpFrameDlg->DoModal();
	pMainDlg->pJumpFrameDlg		= NULL;
}

void CImageDlg::OnMenuitemGosameframe() 
{
	// TODO: Add your command handler code here
    uint8	bFindSameFrame;
    int32   s32FrameNr;
    uint32	i;
    uint32  u32LumaLen;
    uint32  u32ChroLen;

    int32           *p32TwinY	= NULL;
    int32           *p32CurrY   = NULL;
    int32           *p32TwinU   = NULL;
    int32           *p32CurrU   = NULL;
    int32           *p32TwinV   = NULL;
    int32           *p32CurrV   = NULL;
	CYUVPlayerDlg	*pMainDlg	= (CYUVPlayerDlg *)this->pMainDlg;
    CImageDlg	    *pCurrImg	= NULL;
    CImageDlg	    *pTwinImg	= NULL;


    u32LumaLen	= (u32LumaPicSize + 3) >> 2;
    u32ChroLen	= (u32ChroPicSize + 3) >> 2;
    pCurrImg	= pMainDlg->pImage[s8DlgIdx];
    pTwinImg	= pMainDlg->pImage[1 - s8DlgIdx];
	p32TwinY	= (int32 *)pTwinImg->pOrigYUV[0];
	p32CurrY	= (int32 *)pCurrImg->pOrigYUV[0];
	p32TwinU	= (int32 *)pTwinImg->pOrigYUV[1];
	p32CurrU	= (int32 *)pCurrImg->pOrigYUV[1];
	p32TwinV	= (int32 *)pTwinImg->pOrigYUV[2];
	p32CurrV	= (int32 *)pCurrImg->pOrigYUV[2];

	for (s32FrameNr = pCurrImg->s32CurrFrameNr; s32FrameNr < pCurrImg->s32FrameNum; s32FrameNr ++)
	{
		bFindSameFrame	 = TRUE;
		pCurrImg->read_one_frame(IMAGE_INVALID);

		for (i = 0; i < u32LumaLen; i++)
		{
			if (p32TwinY[i] != p32CurrY[i])
			{
				bFindSameFrame	 = FALSE;

				break;
			}
		}	

		if (bFindSameFrame == TRUE)
		{
			for (i = 0; i < u32ChroLen; i++)
			{
				if (p32TwinU[i] != p32CurrU[i])
				{
					bFindSameFrame	 = FALSE;

					break;
				}
				
				if (p32TwinV[i] != p32TwinV[i])
				{
					bFindSameFrame	 = FALSE;

					break;
				}
			}	
		}
		
		if (bFindSameFrame == TRUE)
		{
			break;
		}
	}

	if (bFindSameFrame == FALSE)
	{
		CString	csMessage;

		csMessage.Format(" 第 %d 帧相同的图像！", pTwinImg->s32CurrFrameNr);
		AfxMessageBox("在当前图像以后没有找到与 " + pTwinImg->fileName + csMessage, MB_ICONINFORMATION);
        jump_to_frame(pMainDlg->u8ImageMode, pCurrImg->s32CurrFrameNr);
	}
	else
	{
		pCurrImg->s32CurrFrameNr	 = s32FrameNr + 1;
		pCurrImg->color_space_convert(pMainDlg->u8ImageMode);
		pCurrImg->InvalidateRect(NULL, FALSE);
		pCurrImg->UpdateWindow();
		pMainDlg->diffPic.show_one_frame(pMainDlg->u8ImageMode, TRUE);
	}
}

void CImageDlg::OnMenuitemYUV() 
{
	// TODO: Add your command handler code here
    set_image_mode(IMAGE_YUV);
}

void CImageDlg::OnMenuitemY() 
{
	// TODO: Add your command handler code here
	set_image_mode(IMAGE_Y);
}

void CImageDlg::OnMenuitemU() 
{
	// TODO: Add your command handler code here
    set_image_mode(IMAGE_U);
}

void CImageDlg::OnMenuitemV() 
{
	// TODO: Add your command handler code here
    set_image_mode(IMAGE_V);
}

void CImageDlg::OnMenuitemNotice() 
{
	// TODO: Add your command handler code here
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	pMainDlg->bNoticeFlag	= !pMainDlg->bNoticeFlag;
}

void CImageDlg::OnMenuitemRotate90() 
{
	// TODO: Add your command handler code here
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	if (pMainDlg->u8PlayMode == COMP_MODE)
	{
		AfxMessageBox("两图对比模式不允许旋转图像！", MB_ICONERROR);
		
		return;
	}
	
	//++ 将图像调整的顺序统一为先旋转后镜像
    switch(u8MirrorMode)
    {
    case MIRROR_HORI:
		switch(s16RotateAngle)
		{
		case 0:
			s16RotateAngle	= 90;
			u8MirrorMode	= MIRROR_VERT;
			break;
		case 90:
			s16RotateAngle	= 0;
			u8MirrorMode	= MIRROR_HORI;
			break;
		case 180:
			s16RotateAngle	= 90;
			u8MirrorMode	= MIRROR_HORI;
			break;
		case 270:
			s16RotateAngle	= 0;
			u8MirrorMode	= MIRROR_VERT;
			break;
		default:
			s16RotateAngle	= (s16RotateAngle + 90) % 360;
			break;
		}
        break;
        
    case MIRROR_VERT:
		switch(s16RotateAngle)
		{
		case 0:
			s16RotateAngle	= 90;
			u8MirrorMode	= MIRROR_HORI;
			break;
		case 90:
			s16RotateAngle	= 0;
			u8MirrorMode	= MIRROR_VERT;
			break;
		case 180:
			s16RotateAngle	= 90;
			u8MirrorMode	= MIRROR_VERT;
			break;
		case 270:
			s16RotateAngle	= 0;
			u8MirrorMode	= MIRROR_HORI;
			break;
		default:
			s16RotateAngle	= (s16RotateAngle + 90) % 360;
			break;
		}
        break;
        
	case MIRROR_BOTH:
		switch(s16RotateAngle)
		{
		case 0:
			s16RotateAngle	= 270;
			u8MirrorMode	= MIRROR_NONE;
			break;
		case 90:
			s16RotateAngle	= 0;
			u8MirrorMode	= MIRROR_NONE;
			break;
		case 180:
			s16RotateAngle	= 90;
			u8MirrorMode	= MIRROR_NONE;
			break;
		case 270:
			s16RotateAngle	= 180;
			u8MirrorMode	= MIRROR_NONE;
			break;
		default:
			s16RotateAngle	= (s16RotateAngle + 90) % 360;
			break;
		}
		break;
                
    default:
		s16RotateAngle	= (s16RotateAngle + 90) % 360;
        break;
    }

	//++ 通知主窗口重新打开当前图像后调整当前图像
	::PostMessage(pMainDlg->m_hWnd, WM_MYMESSAGE_4, s8DlgIdx, 0);
}

void CImageDlg::OnMenuitemRotate270() 
{
	// TODO: Add your command handler code here
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;
	
	
	if (pMainDlg->u8PlayMode == COMP_MODE)
	{
		AfxMessageBox("两图对比模式不允许旋转图像！", MB_ICONERROR);
		
		return;
	}
	
	//++ 将图像调整的顺序统一为先旋转后镜像
    switch(u8MirrorMode)
    {
    case MIRROR_HORI:
		switch(s16RotateAngle)
		{
		case 0:
			s16RotateAngle	= 90;
			u8MirrorMode	= MIRROR_HORI;
			break;
		case 90:
			s16RotateAngle	= 0;
			u8MirrorMode	= MIRROR_VERT;
			break;
		case 180:
			s16RotateAngle	= 270;
			u8MirrorMode	= MIRROR_HORI;
			break;
		case 270:
			s16RotateAngle	= 0;
			u8MirrorMode	= MIRROR_HORI;
			break;
		default:
			s16RotateAngle	= (s16RotateAngle + 270) % 360;
			break;
		}
        break;
        
    case MIRROR_VERT:
		switch(s16RotateAngle)
		{
		case 0:
			s16RotateAngle	= 270;
			u8MirrorMode	= MIRROR_HORI;
			break;
		case 90:
			s16RotateAngle	= 0;
			u8MirrorMode	= MIRROR_HORI;
			break;
		case 180:
			s16RotateAngle	= 90;
			u8MirrorMode	= MIRROR_HORI;
			break;
		case 270:
			s16RotateAngle	= 0;
			u8MirrorMode	= MIRROR_VERT;
			break;
		default:
			s16RotateAngle	= (s16RotateAngle + 270) % 360;
			break;
		}
        break;
        
	case MIRROR_BOTH:
		switch(s16RotateAngle)
		{
		case 0:
			s16RotateAngle	= 90;
			u8MirrorMode	= MIRROR_NONE;
			break;
		case 90:
			s16RotateAngle	= 180;
			u8MirrorMode	= MIRROR_NONE;
			break;
		case 180:
			s16RotateAngle	= 270;
			u8MirrorMode	= MIRROR_NONE;
			break;
		case 270:
			s16RotateAngle	= 0;
			u8MirrorMode	= MIRROR_NONE;
			break;
		default:
			s16RotateAngle	= (s16RotateAngle + 270) % 360;
			break;
		}
		break;
                
    default:
		s16RotateAngle	= (s16RotateAngle + 270) % 360;
        break;
    }
	
	//++ 通知主窗口重新打开当前图像后调整当前图像
	::PostMessage(pMainDlg->m_hWnd, WM_MYMESSAGE_4, s8DlgIdx, 0);
}

void CImageDlg::OnMenuitemMirrorHori() 
{
	// TODO: Add your command handler code here
	uint8	u8OldMode;


	u8OldMode		= u8MirrorMode & MIRROR_HORI;
	u8MirrorMode	= (u8MirrorMode & (~MIRROR_HORI)) | (u8OldMode ^ MIRROR_HORI);

/*
	if (u8OldMode)
	{
		pSubMenu->CheckMenuItem(ID_MENUITEM_MIRROR_HORI, MF_BYCOMMAND | MF_UNCHECKED);
	}
	else
	{
		pSubMenu->CheckMenuItem(ID_MENUITEM_MIRROR_HORI, MF_BYCOMMAND | MF_CHECKED);
	}
*/

	//++ 通知主窗口重新打开当前图像后调整当前图像
	::PostMessage(pMainDlg->m_hWnd, WM_MYMESSAGE_4, s8DlgIdx, 1);
}

void CImageDlg::OnMenuitemMirrorVert() 
{
	// TODO: Add your command handler code here
	uint8	u8OldMode;
	
	
	u8OldMode		= u8MirrorMode & MIRROR_VERT;
	u8MirrorMode	= (u8MirrorMode & (~MIRROR_VERT)) | (u8OldMode ^ MIRROR_VERT);
	
/*
	if (u8OldMode)
	{
		pSubMenu->CheckMenuItem(ID_MENUITEM_MIRROR_VERT, MF_BYCOMMAND | MF_UNCHECKED);
	}
	else
	{
		pSubMenu->CheckMenuItem(ID_MENUITEM_MIRROR_VERT, MF_BYCOMMAND | MF_CHECKED);
	}
*/

	//++ 通知主窗口重新打开当前图像后调整当前图像
	::PostMessage(pMainDlg->m_hWnd, WM_MYMESSAGE_4, s8DlgIdx, 1);
}

void CImageDlg::OnMenuitemShowmbinfo() 
{
	// TODO: Add your command handler code here
    enable_mbinfo_dlg();
}

void CImageDlg::OnMenuitemAttach() 
{
	// TODO: Add your command handler code here
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;
	
	
	pMainDlg->bAttachFlag	= !pMainDlg->bAttachFlag;
}

void CImageDlg::OnClose() 
{
    // TODO: Add your message handler code here and/or call default
    CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;
    
    
	bCloseFlag		 = TRUE;
	if (pMainDlg->get_play_status() != PLAY_STATUS)
	{
		pMainDlg->close_image(s8DlgIdx);
	}
	else
	{
		pMainDlg->s8StopValue	 = PAUSE_VALUE_5;
		pMainDlg->s8PlusValue	 = s8DlgIdx;
		pMainDlg->set_play_status(PAUSE_STATUS);
	}
}

void CImageDlg::free_resource()
{
	if (pFile->m_hFile != pFile->hFileNull)
	{
		pFile->Close();
		delete(pFile);
	}

	free(pYUVBuff);
	GlobalFree(hloc);

	MBInfoDlg.talbeFont.DeleteObject();
	MBInfoDlg.DestroyWindow();
	DestroyWindow();
}

void CImageDlg::OnSizing(UINT fwSide, LPRECT pRect) 
{
	CDialog::OnSizing(fwSide, pRect);
	
	// TODO: Add your message handler code here
	bSizeChanged	= TRUE;
	change_size(pRect);
}

LRESULT CImageDlg::OnEnterSizeMove(WPARAM wp, LPARAM lp)
{
    GetWindowRect(&wndRect);
    oldSizeRect		= wndRect;
	
	return TRUE;
}

LRESULT CImageDlg::OnExitSizeMove(WPARAM wp, LPARAM lp)
{
	if (bSizeChanged == TRUE)
	{
		update_image();

		bSizeChanged	= FALSE;
	}

	return TRUE;
}

void CImageDlg::OnMoving(UINT fwSide, LPRECT pRect) 
{
	CDialog::OnMoving(fwSide, pRect);
	
	// TODO: Add your message handler code here
	change_location(pRect);
}

BOOL CImageDlg::PreTranslateMessage(MSG* pMsg) 
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
