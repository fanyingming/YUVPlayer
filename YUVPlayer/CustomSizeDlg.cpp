// CustomSizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CustomSizeDlg.h"
#include "YUVPlayerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomSizeDlg dialog


CCustomSizeDlg::CCustomSizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCustomSizeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCustomSizeDlg)
	m_sWidth		= _T("");
	m_sHeight		= _T("");
	m_sPriority		= _T("");
	//}}AFX_DATA_INIT
}

CCustomSizeDlg::CCustomSizeDlg(CWnd* pParent, CWnd* pMainDlg)
	: CDialog(CCustomSizeDlg::IDD, pParent)
{
	m_sWidth		= _T("");
	m_sHeight		= _T("");
	m_sPriority		= _T("");
	this->pMainDlg		= pMainDlg;
}


void CCustomSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomSizeDlg)
	DDX_Control(pDX, IDC_WIDTH, m_width);
	DDX_Control(pDX, IDC_SIZE_LIST, m_sizeList);
	DDX_Text(pDX, IDC_HEIGHT, m_sHeight);
	DDX_Text(pDX, IDC_WIDTH, m_sWidth);
	DDX_Text(pDX, IDC_PRIORITY, m_sPriority);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCustomSizeDlg, CDialog)
	//{{AFX_MSG_MAP(CCustomSizeDlg)
	ON_BN_CLICKED(IDC_DELETE_ITEMS, OnDeleteItems)
	ON_BN_CLICKED(IDC_NO_SELECT, OnNoSelect)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CLICK, IDC_SIZE_LIST, OnClickSizeList)
	ON_BN_CLICKED(IDC_CLEAR_SIZE, OnClearSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomSizeDlg message handlers

BOOL CCustomSizeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	initial();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCustomSizeDlg::initial()
{
	DWORD	dwStyle;
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	dwStyle		 = m_sizeList.GetExtendedStyle();
    dwStyle		|= LVS_EX_FULLROWSELECT;	//++ 选中某行使整行高亮
    dwStyle		|= LVS_EX_GRIDLINES;		//++ 网格线
	m_sizeList.SetExtendedStyle(dwStyle);
	m_sizeList.InsertColumn(0, _T("尺  寸"), LVCFMT_CENTER, 80);
	m_sizeList.InsertColumn(1, _T("优先级"), LVCFMT_CENTER, 50);
	get_size_list();

	pMainDlg->UpdateData(TRUE);
}

void CCustomSizeDlg::get_size_list()
{
	int32	i;
	int32	s32ItemNum;
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	m_sizeList.DeleteAllItems();
	s32ItemNum	 = pMainDlg->configure.get_size_num();
	for (i = 0; i < s32ItemNum; i ++)
	{
		uint32		u32Priority;
		CString		item;

		item	 = pMainDlg->configure.get_framesize(i);
		m_sizeList.InsertItem(i, item);

		u32Priority	 = pMainDlg->configure.get_priority(i);
		item.Format("%d", u32Priority); 
		m_sizeList.SetItemText(i, 1, item);
	}

	reset_input();
}

void CCustomSizeDlg::set_frame_size(int8 s8Offset)
{
	int32		s32Item;
	int32		s32Pos;
	int32		s32Len;
	CString		currItem;
	POSITION	pos	 = NULL;

	
	pos			= m_sizeList.GetFirstSelectedItemPosition();
	s32Item		= m_sizeList.GetNextSelectedItem(pos);
	currItem	= m_sizeList.GetItemText(s32Item + s8Offset, 0);
	m_sPriority	= m_sizeList.GetItemText(s32Item + s8Offset, 1);
	
	s32Pos		= currItem.Find('x');
	s32Len		= currItem.GetLength();
	m_sWidth	= currItem.Left(s32Pos);
	m_sHeight	= currItem.Mid(s32Pos + 1, s32Len - s32Pos - 1);

	UpdateData(FALSE);
}

int32 CCustomSizeDlg::get_frame_size()
{
	int32	s32Width;
	int32	s32Height;
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;
	

	UpdateData(TRUE);
	s32Width	 = atoi(m_sWidth);
	s32Height	 = atoi(m_sHeight);

	if ((s32Height <= 0) || s32Width <= 0)
	{
//		AfxMessageBox("自定义图像尺寸无效！", MB_ICONERROR);
		
		return FAILED_YUVPlayer;
	}

	frameSize.Format("%dx%d", s32Width, s32Height);

	return SUCCEEDED_YUVPlayer;
}

int32 CCustomSizeDlg::check_size()
{
	int32		s32Item;
	LVFINDINFO	info;

	info.flags	= LVFI_PARTIAL;
	info.psz	= frameSize;
	s32Item		= m_sizeList.FindItem(&info, -1);
	if (s32Item != -1)	//++ 如果该尺寸已经存在，则只更新其优先级
	{
		m_sizeList.SetItemText(s32Item, 1, m_sPriority);

		return FAILED_YUVPlayer;
	}

	return SUCCEEDED_YUVPlayer;
}

void CCustomSizeDlg::reset_input()
{
	m_sWidth	 = "";
	m_sHeight	 = "";
	m_sPriority	 = "";

	UpdateData(FALSE);
	m_width.SetFocus();
}

void CCustomSizeDlg::add_item() 
{
	// TODO: Add your control notification handler code here
    int32   s32Ret;
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	s32Ret	 = get_frame_size();
	if (s32Ret == FAILED_YUVPlayer)
	{
		reset_input();

		return;
	}
	s32Ret	 = frameSize.GetLength();
	if (s32Ret >= MAX_NAME_LEN)
	{
		AfxMessageBox("图像尺寸异常！", MB_ICONERROR);
		reset_input();

		return;
	}
	if (m_sPriority == "")
	{
		m_sPriority		= _T("0");
	}
	//++ 检查尺寸是否已经存在
	s32Ret	 = check_size();
	if (s32Ret == FAILED_YUVPlayer)
	{
		return;
	}

	//++ 加入当前列表中
	m_sizeList.InsertItem(0, frameSize);
	m_sizeList.SetItemText(0, 1, m_sPriority);
}

void CCustomSizeDlg::OnClearSize() 
{
	// TODO: Add your control notification handler code here
	reset_input();
}

void CCustomSizeDlg::OnDeleteItems() 
{
	// TODO: Add your control notification handler code here
	int32		s32ItemNum;
	uint32		i;
	uint32		u32ItemNum;
	POSITION	pos	 = NULL;
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;
	

	s32ItemNum	 = m_sizeList.GetItemCount();
	u32ItemNum	 = m_sizeList.GetSelectedCount();
	if (((uint32)s32ItemNum) == u32ItemNum)
	{
		AfxMessageBox("禁止清空列表！", MB_ICONWARNING);
		OnNoSelect();

		return;
	}
	
	for (i = 0; i < u32ItemNum; i ++)
	{
		pos	 = m_sizeList.GetFirstSelectedItemPosition();
		
		int32	s32Item	 = m_sizeList.GetNextSelectedItem(pos);
		CString	item0	 = m_sizeList.GetItemText(s32Item, 0);
		CString	item1	 = m_sizeList.GetItemText(s32Item, 1);
		
		//++ 从当前列表中删除
		m_sizeList.DeleteItem(s32Item);
	}

	reset_input();
}

void CCustomSizeDlg::OnNoSelect() 
{
	// TODO: Add your control notification handler code here
	int32	i;
	int32	s32ItemNum;


	s32ItemNum	 = m_sizeList.GetItemCount();
	for (i = 0; i < s32ItemNum; i ++)
	{
		m_sizeList.SetItemState(i, 0, LVIS_SELECTED);
	}
}

void CCustomSizeDlg::update_framesize_list()
{
	int32	s32ItemNum;
	CYUVPlayerDlg	*pMainDlg	= (CYUVPlayerDlg *)this->pMainDlg;


	pMainDlg->configure.reset_list();
	pMainDlg->m_frameSize.ResetContent();

	s32ItemNum	 = m_sizeList.GetItemCount();
	for (int32 i = 0; i < s32ItemNum; i ++)
	{
		int8	priorityItem[MAX_NAME_LEN];
		int8	sizeItem[MAX_NAME_LEN];

		m_sizeList.GetItemText(i, 0, sizeItem, MAX_NAME_LEN);
		m_sizeList.GetItemText(i, 1, priorityItem, MAX_NAME_LEN);
		//++ 将数据更新到主窗口列表中
		pMainDlg->m_frameSize.AddString(sizeItem);
		//++ 将数据更新到配置文件缓存中
		pMainDlg->configure.insert_item(i, atoi(priorityItem), sizeItem);
	}
}

void CCustomSizeDlg::OnOK() 
{
	// TODO: Add extra validation here
	int32	s32Width;
	int32	s32Height;
	CYUVPlayerDlg	*pMainDlg	= (CYUVPlayerDlg *)this->pMainDlg;

	
	add_item();
	update_framesize_list();

	pMainDlg->m_customSize.SetCheck(BST_UNCHECKED);
	pMainDlg->m_openFile.SetFocus();
	
	if ((pMainDlg->u8PlayMode == VIEW_MODE) || (pMainDlg->s8ImgNum == 0))
	{
		pMainDlg->m_frameSize.EnableWindow(TRUE);
		
		UpdateData(TRUE);
		s32Width	 = atoi(m_sWidth);
		s32Height	 = atoi(m_sHeight);
		if ((s32Width > 0) && (s32Height > 0))
		{
			CString	sizeItem;
			
			sizeItem.Format("%dx%d", s32Width, s32Height);
			pMainDlg->m_sFrameSize	 = sizeItem;
		}
	}
	
	pMainDlg->UpdateData(FALSE);

	CDialog::OnOK();
}

void CCustomSizeDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;

	
	pMainDlg->m_customSize.SetCheck(BST_UNCHECKED);
	pMainDlg->m_openFile.SetFocus();
	if ((pMainDlg->u8PlayMode == VIEW_MODE) || (pMainDlg->s8ImgNum == 0))
	{
		pMainDlg->m_frameSize.EnableWindow(TRUE);
	}

	CDialog::OnCancel();
}

void CCustomSizeDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	OnCancel();
}

void CCustomSizeDlg::close_dlg() 
{
	// TODO: Add your message handler code here and/or call default
	OnCancel();
}

void CCustomSizeDlg::OnClickSizeList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	set_frame_size(0);
	
	*pResult = 0;
}

BOOL CCustomSizeDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN)  //++ 判断是否有按键按下
    {
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
			OnCancel();
				
			return TRUE;
			break;

		case VK_RETURN:
			OnOK();

			return TRUE;
			break;

		case VK_UP:
			{
				CWnd	*pWnd	 = GetFocus();

				if (pWnd != NULL)
				{
					if (pWnd == GetDlgItem(IDC_SIZE_LIST))
					{
						set_frame_size(-1);
					}
					else if (pWnd == GetDlgItem(IDC_WIDTH))
					{
					}
					else if (pWnd == GetDlgItem(IDC_HEIGHT))
					{
					}
					else if (pWnd == GetDlgItem(IDC_PRIORITY))
					{
					}
					
					UpdateData(FALSE);
				}
			}

			break;
			
		case VK_DOWN:
			{
				CWnd	*pWnd	 = GetFocus();

				if (pWnd != NULL)
				{
					if (pWnd == GetDlgItem(IDC_SIZE_LIST))
					{
						set_frame_size(1);
					}
					else if (pWnd == GetDlgItem(IDC_WIDTH))
					{
					}
					else if (pWnd == GetDlgItem(IDC_HEIGHT))
					{
					}
					else if (pWnd == GetDlgItem(IDC_PRIORITY))
					{
					}
					
					UpdateData(FALSE);
				}
			}

			break;

		case VK_C:
			OnClearSize();

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
