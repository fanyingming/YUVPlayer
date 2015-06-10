// JumpFrameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JumpFrameDlg.h"
#include "YUVPlayerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJumpFrameDlg dialog


CJumpFrameDlg::CJumpFrameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJumpFrameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJumpFrameDlg)
	m_sJumpFrameNr = _T("");
	pParentWnd		= pParent;
	//}}AFX_DATA_INIT
}

CJumpFrameDlg::CJumpFrameDlg(CWnd* pParent, CWnd* pMainDlg)
	: CDialog(CJumpFrameDlg::IDD, pParent)
{
	m_sJumpFrameNr = _T("");
	pParentWnd		= pMainDlg;
}


void CJumpFrameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJumpFrameDlg)
	DDX_Control(pDX, IDC_CURR_JUMP, m_currJump);
	DDX_Control(pDX, IDC_FRAME_NUMBER, m_frameNr);
	DDX_Text(pDX, IDC_FRAME_NUMBER, m_sJumpFrameNr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJumpFrameDlg, CDialog)
	//{{AFX_MSG_MAP(CJumpFrameDlg)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_ALL_JUMP, OnAllJump)
	ON_BN_CLICKED(IDC_CURR_JUMP, OnCurrJump)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJumpFrameDlg message handlers

void CJumpFrameDlg::initial()
{
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)pParentWnd;

	
	m_sJumpFrameNr	= _T("");
	UpdateData(FALSE);
	
	m_frameNr.SetFocus();
	m_currJump.EnableWindow(!bDiffDlgFlag);
}

BOOL CJumpFrameDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	initial();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int32 CJumpFrameDlg::get_frame_number() 
{
	int32	s32JumpFrameNr;


	UpdateData(TRUE);
	s32JumpFrameNr	 = atoi(m_sJumpFrameNr);

	return s32JumpFrameNr;
}

void CJumpFrameDlg::OnCurrJump() 
{
	// TODO: Add your control notification handler code here
	int32	s32JumpFrameNr;
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)pParentWnd;
	
	
	s32JumpFrameNr	 = get_frame_number();
	if (s32JumpFrameNr <= 0)
	{
		AfxMessageBox("指定帧号无效，最小帧号为 1！", MB_ICONERROR);
		OnClear();
		
		return;
	}
	
	if (s32JumpFrameNr > pCurrJumpImg->s32FrameNum)
	{
		AfxMessageBox("指定帧号超过 " + pCurrJumpImg->fileName + " 的最大帧数，跳转失败！", MB_ICONERROR);
		OnClear();
		
		return;
	}
	else
	{
		pMainDlg->u8EOFNum	= (pMainDlg->u8EOFNum > 0) ? (-- pMainDlg->u8EOFNum) : 0;
		pMainDlg->u8EOHNum	= (pMainDlg->u8EOHNum > 0) ? (-- pMainDlg->u8EOHNum) : 0;
		pCurrJumpImg->jump_to_frame(pMainDlg->u8ImageMode, s32JumpFrameNr);
	}
	
	if (pMainDlg->bShowDiffPic == TRUE)
	{
		pMainDlg->diffPic.show_one_frame(pMainDlg->u8ImageMode, TRUE);
	}

	OnOK();
}

void CJumpFrameDlg::OnAllJump() 
{
	// TODO: Add your control notification handler code here
	int32	i;
	int32	s32JumpFrameNr;
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)pParentWnd;
	
	
	s32JumpFrameNr	 = get_frame_number();
	if (s32JumpFrameNr <= 0)
	{
		AfxMessageBox("指定帧号无效，最小帧号为 1！", MB_ICONERROR);
		OnClear();
		
		return;
	}
	
	for (i = 0; i < pMainDlg->s8ImgNum; i ++)
	{
		CImageDlg	*pCurrImg	 = pMainDlg->pImage[i];
		
		if ((s32JumpFrameNr) > pCurrImg->s32FrameNum)
		{
			AfxMessageBox("指定帧号超过 " + pCurrImg->fileName + " 的最大帧数，跳转失败！", MB_ICONERROR);
			OnClear();
			
			return;
		}
	}
	
	for (i = 0; i < pMainDlg->s8ImgNum; i ++)
	{
		pMainDlg->u8EOFNum	= (pMainDlg->u8EOFNum > 0) ? (-- pMainDlg->u8EOFNum) : 0;
		pMainDlg->u8EOHNum	= (pMainDlg->u8EOHNum > 0) ? (-- pMainDlg->u8EOHNum) : 0;
		pMainDlg->pImage[i]->jump_to_frame(pMainDlg->u8ImageMode, s32JumpFrameNr);
	}
	
	if (pMainDlg->bShowDiffPic == TRUE)
	{
		pMainDlg->diffPic.show_one_frame(pMainDlg->u8ImageMode, TRUE);
	}
	
	OnOK();
}

void CJumpFrameDlg::OnClear() 
{
	// TODO: Add your control notification handler code here
	m_sJumpFrameNr	= _T("");
	
	UpdateData(FALSE);
	m_frameNr.SetFocus();
}

void CJumpFrameDlg::close_dlg() 
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

BOOL CJumpFrameDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	int32	s32JumpFrameNr;


	if(pMsg->message == WM_KEYDOWN)  //++ 判断是否有按键按下
    {
		CYUVPlayerDlg*	pMainDlg	= (CYUVPlayerDlg *)pParentWnd;
		
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
			OnCancel();

			return TRUE;
			break;

		case VK_RETURN:
			if (pMainDlg->u8PlayMode == VIEW_MODE)
			{
				OnCurrJump();
			}
			else
			{
				OnAllJump();
			}

			return TRUE;
			break;

		case VK_UP:
			s32JumpFrameNr	 = get_frame_number();
			if (s32JumpFrameNr < s32MaxFrameNum)
			{
				s32JumpFrameNr ++;
			}
			else
			{
				s32JumpFrameNr	 = 1;
			}
			
			m_sJumpFrameNr.Format("%d", s32JumpFrameNr);
			UpdateData(FALSE);
			
			return TRUE;
			break;
			
		case VK_DOWN:
			s32JumpFrameNr	 = get_frame_number();
			if (s32JumpFrameNr > 1)
			{
				s32JumpFrameNr --;
			}
			else
			{
				s32JumpFrameNr	 = s32MaxFrameNum;
			}

			m_sJumpFrameNr.Format("%d", s32JumpFrameNr);
			UpdateData(FALSE);
			
			return TRUE;
			break;

		case VK_DELETE:
		case VK_BACK:
		case VK_TAB:
		case VK_LEFT:
		case VK_RIGHT:

			break;
			
		default:
			if (((pMsg->wParam < VK_0) || (pMsg->wParam > VK_9)) && ((pMsg->wParam < VK_NUMPAD0) || (pMsg->wParam > VK_NUMPAD9)))
			{
				return TRUE;
			}
            
			break;
		}
	}
	else if (pMsg->message == WM_RBUTTONDOWN)
	{
		return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
