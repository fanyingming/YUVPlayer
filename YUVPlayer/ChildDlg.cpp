#include "stdafx.h"
#include "ChildDlg.h"
#include "YUVPlayerDlg.h"
#include "multithread.h"


CChildDlg::CChildDlg(UINT nID, CWnd* pParent /*=NULL*/)
	: CDialog(nID, pParent)
{
	//++ 将需要标记的宏块的坐标初始化到图像区域外
    s32MBXIdx			= 0;		//++ 当前宏块所在列号（缩放前）
    s32MBYIdx			= 0;		//++ 当前宏块所在行号（缩放前）
    s32ViewMBx			= -100;		//++ 显示像素值的宏块的顶点坐标（缩放前）
    s32ViewMBy			= -100;		//++ 显示像素值的宏块的顶点坐标（缩放前）
    s32ViewBlkX			= -100;		//++ 显示像素值的宏块的顶点坐标（缩放后）
    s32ViewBlkY			= -100;		//++ 显示像素值的宏块的顶点坐标（缩放后）
	s32ViewBlkW			= 0;
	s32ViewBlkH			= 0;
    s32PrevBlkX			= -100;		//++ 前一个鼠标指向的宏块的顶点坐标（缩放后）
    s32PrevBlkY			= -100;		//++ 前一个鼠标指向的宏块的顶点坐标（缩放后）
	s32PrevBlkW			= 0;
	s32PrevBlkH			= 0;
    s32CurrMBx			= -100;		//++ 鼠标经过的宏块的顶点坐标（缩放前）
    s32CurrMBy			= -100;		//++ 鼠标经过的宏块的顶点坐标（缩放前）
    s32CurrBlkX			= -100;		//++ 鼠标经过的宏块的顶点坐标（缩放后）
    s32CurrBlkY			= -100;		//++ 鼠标经过的宏块的顶点坐标（缩放后）
	s32CurrBlkW			= 0;
	s32CurrBlkH			= 0;
	bSizeChanged		= FALSE;
	pReadYUV[0]			= NULL;
	pReadYUV[1]			= NULL;
	pReadYUV[2]			= NULL;
	pOrigYUV[0]			= NULL;
	pOrigYUV[1]			= NULL;
	pOrigYUV[2]			= NULL;
	pMirrYUV[0]			= NULL;
	pMirrYUV[1]			= NULL;
	pMirrYUV[2]			= NULL;
	pRotaYUV[0]			= NULL;
	pRotaYUV[1]			= NULL;
	pRotaYUV[2]			= NULL;
	pDisplayLuma		= NULL;
	pDisplayChro		= NULL;
	pRGBBuff			= NULL;
	
    mouseMenu.LoadMenu(IDR_MOUSE_MENU);//装载自定义的右键菜单
    pSubMenu   = mouseMenu.GetSubMenu(0);//获取第一个弹出菜单，所以第一个菜单必须有子菜单
}

int32 CChildDlg::malloc_memory()
{
    uint32  u32LumaBuffSize;
    uint32  u32ChroBuffSize;
    uint32	u32MemorySize;
    
    
    //++ 为加快寻找同步帧的速度，原始图像内存空间按 4 字节整数倍开辟
    u32LumaBuffSize	  = ((u32LumaPicSize + 3) >> 2) << 2;
    u32ChroBuffSize	  = ((u32ChroPicSize + 3) >> 2) << 2;
    u32MemorySize	  = u32LumaBuffSize + (u32ChroBuffSize << 1);	//++ 读入的原始 YUV 图像
    u32MemorySize	 += u32LumaBuffSize + (u32ChroBuffSize << 1);	//++ 镜像缓冲
    u32MemorySize	 += u32LumaBuffSize + (u32ChroBuffSize << 1);	//++ 旋转缓冲
    u32MemorySize	 += u32LumaPicSize;	//++ 用于显示的 YUV 图像亮度分量
    u32MemorySize	 += u32ChroPicSize;	//++ 用于显示的 YUV 图像色度分量
    u32MemorySize	 += (((s32Width * 3 + 3) >> 2) << 2) * s32Height;	//++ RGB 内存空间
    
    pYUVBuff    = (LPBYTE)malloc(u32MemorySize);
    if (NULL == pYUVBuff)
    {
        AfxMessageBox("分配内存错误！\n", MB_ICONERROR);
        
        return FAILED_YUVPlayer;
    }
    pReadYUV[0]		= pYUVBuff;
    pReadYUV[1]		= pReadYUV[0] + u32LumaBuffSize;
    pReadYUV[2]		= pReadYUV[1] + u32ChroBuffSize;
    pMirrYUV[0]		= pReadYUV[2] + u32ChroBuffSize;
    pMirrYUV[1]		= pMirrYUV[0] + u32LumaBuffSize;
    pMirrYUV[2]		= pMirrYUV[1] + u32ChroBuffSize;
    pRotaYUV[0]		= pMirrYUV[2] + u32ChroBuffSize;
    pRotaYUV[1]		= pRotaYUV[0] + u32LumaBuffSize;
    pRotaYUV[2]		= pRotaYUV[1] + u32ChroBuffSize;
    pDisplayLuma	= pRotaYUV[2] + u32ChroBuffSize;
    pDisplayChro	= pDisplayLuma + u32LumaPicSize;
    pRGBBuff		= pDisplayChro + u32ChroPicSize;
    memset(pYUVBuff, 128, u32MemorySize);
    
    hloc = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 256));
    if (NULL == hloc)
    {
        free(pYUVBuff);
        AfxMessageBox("分配内存错误！\n", MB_ICONERROR);
        
        return FAILED_YUVPlayer;
    }
    
    return SUCCEEDED_YUVPlayer;
}

void CChildDlg::set_bmp_parameter()
{
    int32	i;
    HANDLE	hloc1;
    RGBQUAD	*argbq;
    
    
    BmpInfo	 = (LPBITMAPINFO) GlobalLock(hloc);
    hloc1	 = LocalAlloc(LMEM_ZEROINIT | LMEM_MOVEABLE,(sizeof(RGBQUAD) * 256));
    argbq	 = (RGBQUAD *) LocalLock(hloc1);
    
    for (i = 0; i < 256; i ++)
    {
        argbq[i].rgbBlue		 = i;
        argbq[i].rgbGreen		 = i;
        argbq[i].rgbRed			 = i;
        argbq[i].rgbReserved	 = 0;
    }
    
    BmpInfo->bmiHeader.biSize			 = sizeof(BITMAPINFOHEADER);
    BmpInfo->bmiHeader.biPlanes			 = 1;
    BmpInfo->bmiHeader.biBitCount		 = 24;
    BmpInfo->bmiHeader.biCompression	 = BI_RGB;
    BmpInfo->bmiHeader.biWidth			 = s32Width;
    BmpInfo->bmiHeader.biHeight			 = s32Height;
    
    memcpy(BmpInfo->bmiColors, argbq, sizeof(RGBQUAD) * 256);
    
    LocalUnlock(hloc1);
    LocalFree(hloc1);
}

void CChildDlg::resize_window()
{
    //++ 将窗口尺寸映射为屏幕尺寸
    CRect	crRect(0, 0, s32ZoomWidth, s32ZoomHeight);
    CalcWindowRect(&crRect, CWnd::adjustOutside);
	SetWindowPos(NULL, 0, 0, crRect.Width(), crRect.Height(), SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_HIDEWINDOW);
}

int32 CChildDlg::show_image(CDC *pDC)
{
    int32   s32Ret;
    
    
    BmpInfo->bmiHeader.biBitCount    = 24;
    pDC->SetStretchBltMode(COLORONCOLOR);
    s32Ret	 = StretchDIBits(pDC->m_hDC, 0, 0, s32ZoomWidth, s32ZoomHeight, 0, 0, s32Width, s32Height, pRGBBuff, BmpInfo, DIB_RGB_COLORS, SRCCOPY);
    
    if (s32Ret == GDI_ERROR)
    {
        return FAILED_YUVPlayer;
    }
    
    return SUCCEEDED_YUVPlayer;
}

void CChildDlg::color_space_convert(uint8 u8ImageMode)
{
    int32	i;
    int32	j;
    int32	s32ChroWidth;
    int32	s32ChroHeigth;

    
    switch (u8SampleFormat)
    {
    case YUV400:
    case YUV420:
        s32ChroWidth    = s32Width >> 1;
        s32ChroHeigth   = s32Height >> 1;

        break;

    default:

        break;
    }
    
    switch(u8ImageMode)
    {
    case IMAGE_YUV:
        YV12_to_RGB24(pOrigYUV[0], pOrigYUV[1], pOrigYUV[2]);

        break;
        
    case IMAGE_Y:
        YV12_to_RGB24(pOrigYUV[0], pDisplayChro, pDisplayChro);
        break;
        
    case IMAGE_U:
        for (j = 0; j < s32Height; j ++)
        {
            for (i = 0; i < s32Width; i ++)
            {
                pDisplayLuma[j * s32Width + i]	 = pOrigYUV[1][(j >> 1) * s32ChroWidth + (i >> 1)];
            }
        }
        YV12_to_RGB24(pDisplayLuma, pDisplayChro, pDisplayChro);

        break;
        
    case IMAGE_V:
        for (j = 0; j < s32Height; j ++)
        {
            for (i = 0; i < s32Width; i ++)
            {
                pDisplayLuma[j * s32Width + i]	 = pOrigYUV[2][(j >> 1) * s32ChroWidth + (i >> 1)];
            }
        }
        YV12_to_RGB24(pDisplayLuma, pDisplayChro, pDisplayChro);

        break;
        
    default:

        break;
    }
}

void CChildDlg::YV12_to_RGB24(uint8* pu8Y, uint8* pu8U, uint8* pu8V)
{
	int32	x;
	int32	y;
	int32	k;
	int32	m	= 0;
	int32	n	= 0;
	int32	rgb[3];
	int32	s32RGBBuffStride	 = ((s32Width * 3 + 3) >> 2) << 2;
    CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	k	= s32Height * s32RGBBuffStride;
	
	for(y = 0; y < s32Height; y ++)
	{
		k	-= s32RGBBuffStride;

		for(x = 0; x < s32Width; x ++)
		{
			int32	i;
			int32	j;

			i	= m + x;
			j	= n + (x >> 1);
			rgb[2]	= int32(1.164383 * (pu8Y[i] - 16) + 1.596027 * (pu8V[j] - 128)); // r
			rgb[1]	= int32(1.164383 * (pu8Y[i] - 16) - 0.812968 * (pu8V[j] - 128) - 0.391762 * (pu8U[j] - 128)); // g
			rgb[0]	= int32(1.164383 * (pu8Y[i] - 16) + 2.017232 * (pu8U[j] - 128)); // b			
// 			rgb[2]	= int32(1.164 * (pu8Y[i] - 16) + 1.793 * (pu8V[j] - 128)); // r
// 			rgb[1]	= int32(1.164 * (pu8Y[i] - 16) - 0.534 * (pu8V[j] - 128) - 0.213 * (pu8U[j] - 128)); // g
// 			rgb[0]	= int32(1.164 * (pu8Y[i] - 16) + 2.115 * (pu8U[j] - 128)); // b			

			i	= k + x * 3;
			for(j = 0; j < 3; j ++)
			{
				if((rgb[j] >= 0) && (rgb[j] <= 255))
				{
					pRGBBuff[i + j]		= rgb[j];
				}
				else
				{
					pRGBBuff[i + j]		= (rgb[j] < 0) ? 0 : 255;
				}
			}
		}
		
		m	+= s32Width;
		if(y % 2)
		{
			n	+= (s32Width >> 1);
		}
	}
}

void CChildDlg::YUY2_to_RGB24(uint8 *pu8RGBData, uint8 *pu8YUVData)
{
	int32  R, G, B;
	int32  x, y;
	int32  Y0, U, Y1, V;


	for (y = 0; y < s32Height; y ++)
	{
		for (x = 0; x < s32Width; x += 2)
		{
			Y0 = *pu8YUVData++;
			U  = *pu8YUVData++;
			Y1 = *pu8YUVData++;
			V  = *pu8YUVData++;

			R  = int32(1.164383 * (Y0 - 16) + 1.596027 * (V - 128));
			G  = int32(1.164383 * (Y0 - 16) - 0.812968 * (V - 128) - 0.391762 * (U - 128));
			B  = int32(1.164383 * (Y0 - 16) + 2.017232 * (U - 128));
			if ( R < 0 )
				R = 0;
			if ( R > 255)
				R = 255;
			if ( G < 0 )
				G = 0;
			if ( G > 255)
				G = 255;
			if ( B < 0 )
				B = 0;
			if ( B > 255 )
				B = 255;
			*pu8RGBData++ = (uint8)B;
			*pu8RGBData++ = (uint8)G;
			*pu8RGBData++ = (uint8)R;

			R  = int32(1.164383 * (Y1 - 16) + 1.596027 * (V - 128));
			G  = int32(1.164383 * (Y1 - 16) - 0.812968 * (V - 128) - 0.391762 * (U - 128));
			B  = int32(1.164383 * (Y1 - 16) + 2.017232 * (U - 128));
			if ( R < 0 )
				R = 0;
			if ( R > 255)
				R = 255;
			if ( G < 0 )
				G = 0;
			if ( G > 255)
				G = 255;
			if ( B < 0 )
				B = 0;
			if ( B > 255 )
				B = 255;
			*pu8RGBData++ = (uint8)B;
			*pu8RGBData++ = (uint8)G;
			*pu8RGBData++ = (uint8)R;
		}
	}
}

void CChildDlg::show_macroblock_info()
{
    CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


	if ((pMainDlg->bShowMBInfo == TRUE) && (pMainDlg->bEnMBInfo == TRUE))
	{
		//++ 获得像素值
		get_pixel_value();
		//++ 擦掉历史选定像素点标记
		MBInfoDlg.clean_mark();
		//++ 输出宏块像素信息
		MBInfoDlg.draw_pixel_table();
		//++ 输出宏块位置信息
		MBInfoDlg.m_sMBInfo.Format(" %d\r\n( x , y ) = ( %d , %d )\r\n( x , y ) = ( %d , %d )\r\n( x , y ) = ( %d , %d )",
			s32MBYIdx * (s32Width >> 4) + s32MBXIdx, s32MBXIdx,	s32MBYIdx,
			s32CurrMBx, s32CurrMBy, (s32CurrMBx >> 1), (s32CurrMBy >> 1));
		
		MBInfoDlg.UpdateData(FALSE);
		MBInfoDlg.ShowWindow(SW_SHOW);
		MBInfoDlg.SetWindowText("宏块信息 - " + fileName);
	}
}

void CChildDlg::view_macroblock()
{
    CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;
    
    
    if ((s32CurrBlkX != s32ViewBlkX) || (s32CurrBlkY != s32ViewBlkY))
    {
        //++ 擦掉历史选定宏块的颜色标记
        CClientDC	currDC(this);
        CRect	currRect(s32ViewBlkX, s32ViewBlkY, s32ViewBlkX + s32ViewBlkW, s32ViewBlkY + s32ViewBlkH);
        InvalidateRect(currRect, TRUE);
        
        //++ 选定宏块采用蓝色标记
        CPen	bluePen;
        bluePen.CreatePen(PS_DOT, 1, RGB(0, 200, 255)); //++ 蓝色虚线笔
        currDC.SelectObject(&bluePen);
        currDC.MoveTo(s32CurrBlkX, s32CurrBlkY);
        currDC.LineTo(s32CurrBlkX + s32CurrBlkW - 1, s32CurrBlkY);
        currDC.LineTo(s32CurrBlkX + s32CurrBlkW - 1, s32CurrBlkY + s32CurrBlkH - 1);
        currDC.LineTo(s32CurrBlkX, s32CurrBlkY + s32CurrBlkH - 1);
        currDC.LineTo(s32CurrBlkX, s32CurrBlkY);
        bluePen.DeleteObject();
        
        s32ViewBlkX		= s32CurrBlkX;
        s32ViewBlkY		= s32CurrBlkY;
		s32ViewBlkW		= s32CurrBlkW;
		s32ViewBlkH		= s32CurrBlkH;
    }

	show_macroblock_info();
}

void CChildDlg::mark_macroblock()
{
    CRect	currRect;
    CClientDC	currDC(this);
    CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;
    

	s32CurrMBx		= s32MBXIdx << 4;
	s32CurrMBy		= s32MBYIdx << 4;
	s32CurrBlkX		= s32CurrMBx * s32ZoomWidth / s32Width;
	s32CurrBlkY		= s32CurrMBy * s32ZoomHeight / s32Height;
    
    if ((s32CurrBlkX != s32PrevBlkX) || (s32CurrBlkY != s32PrevBlkY))
    {
        //++ 擦掉历史宏块的颜色标记
        if ((s32ViewBlkX != s32PrevBlkX) || (s32ViewBlkY != s32PrevBlkY))
        {
            currRect.top	 = s32PrevBlkY;
            currRect.left	 = s32PrevBlkX;
            currRect.right	 = s32PrevBlkX + s32PrevBlkW;
            currRect.bottom	 = s32PrevBlkY + s32PrevBlkH;
            InvalidateRect(currRect, TRUE);
        }
        
        //++ 鼠标经过选定宏块之后，将选定宏块恢复为蓝色标记
        if ((pMainDlg->bShowMBInfo == TRUE) && (s32PrevBlkX == s32ViewBlkX) && (s32PrevBlkY == s32ViewBlkY))
        {
            CPen	bluePen;
            bluePen.CreatePen(PS_DOT, 1, RGB(0, 200, 255)); //++ 蓝色虚线笔
            currDC.SelectObject(&bluePen);
            currDC.MoveTo(s32ViewBlkX, s32ViewBlkY);
            currDC.LineTo(s32ViewBlkX + s32ViewBlkW - 1, s32ViewBlkY);
            currDC.LineTo(s32ViewBlkX + s32ViewBlkW - 1, s32ViewBlkY + s32ViewBlkH - 1);
            currDC.LineTo(s32ViewBlkX, s32ViewBlkY + s32ViewBlkH - 1);
            currDC.LineTo(s32ViewBlkX, s32ViewBlkY);
            bluePen.DeleteObject();
        }
        
        //++ 当前宏块采用绿色标记
        CPen	greenPen;
        greenPen.CreatePen(PS_DOT, 1, RGB(0, 255, 0)); //++ 绿色虚线笔
        currDC.SelectObject(&greenPen);

		s32CurrBlkW		= (s32CurrMBx + 16) * s32ZoomWidth / s32Width - s32CurrBlkX;
		s32CurrBlkH		= (s32CurrMBy + 16) * s32ZoomHeight / s32Height - s32CurrBlkY;
        currDC.MoveTo(s32CurrBlkX, s32CurrBlkY);
        currDC.LineTo(s32CurrBlkX + s32CurrBlkW - 1, s32CurrBlkY);
        currDC.LineTo(s32CurrBlkX + s32CurrBlkW - 1, s32CurrBlkY + s32CurrBlkH - 1);
        currDC.LineTo(s32CurrBlkX, s32CurrBlkY + s32CurrBlkH - 1);
        currDC.LineTo(s32CurrBlkX, s32CurrBlkY);
        greenPen.DeleteObject();
        
        s32PrevBlkX		= s32CurrBlkX;
        s32PrevBlkY		= s32CurrBlkY;
		s32PrevBlkW		= s32CurrBlkW;
		s32PrevBlkH		= s32CurrBlkH;
    }
}

void CChildDlg::remark_macroblock(CPaintDC *pDC) 
{
    //++ 当前宏块采用蓝色标记
    CPen	bluePen;
    bluePen.CreatePen(PS_DOT, 1, RGB(0, 200, 255)); //++ 蓝色笔
    pDC->SelectObject(&bluePen);
    pDC->MoveTo(s32ViewBlkX, s32ViewBlkY);
    pDC->LineTo(s32ViewBlkX + s32ViewBlkW - 1, s32ViewBlkY);
    pDC->LineTo(s32ViewBlkX + s32ViewBlkW - 1, s32ViewBlkY + s32ViewBlkH - 1);
    pDC->LineTo(s32ViewBlkX, s32ViewBlkY + s32ViewBlkH - 1);
    pDC->LineTo(s32ViewBlkX, s32ViewBlkY);
    bluePen.DeleteObject();
}

void CChildDlg::show_mouse_menu()
{
    CString	csMBInfo;
    CPoint	cursorPos;//定义一个用于确定光标位置的位置

    CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;

    
    GetCursorPos(&cursorPos);//获取当前光标的位置，以便使得菜单可以跟随光标
    
    csMBInfo.Format("宏块地址：%d", s32MBYIdx * (s32Width >> 4) + s32MBXIdx);
    pSubMenu->ModifyMenu(ID_MENUITEM_MBINFO1, MF_BYCOMMAND, ID_MENUITEM_MBINFO1, csMBInfo);
    csMBInfo.Format("宏块坐标：(%d, %d)", s32MBXIdx, s32MBYIdx);
    pSubMenu->ModifyMenu(ID_MENUITEM_MBINFO2, MF_BYCOMMAND, ID_MENUITEM_MBINFO2, csMBInfo);
    csMBInfo.Format("亮度顶点：(%d, %d)", s32CurrMBx, s32CurrMBy);
    pSubMenu->ModifyMenu(ID_MENUITEM_MBINFO3, MF_BYCOMMAND, ID_MENUITEM_MBINFO3, csMBInfo);
    csMBInfo.Format("色度顶点：(%d, %d)", (s32CurrMBx >> 1), (s32CurrMBy >> 1));
    pSubMenu->ModifyMenu(ID_MENUITEM_MBINFO4, MF_BYCOMMAND, ID_MENUITEM_MBINFO4, csMBInfo);

    switch (pMainDlg->u8ImageMode)
    {
    case IMAGE_YUV:
        pSubMenu->CheckMenuItem(ID_MENUITEM_YUV, MF_BYCOMMAND | MF_CHECKED);
        pSubMenu->CheckMenuItem(ID_MENUITEM_Y, MF_BYCOMMAND | MF_UNCHECKED);
        pSubMenu->CheckMenuItem(ID_MENUITEM_U, MF_BYCOMMAND | MF_UNCHECKED);
        pSubMenu->CheckMenuItem(ID_MENUITEM_V, MF_BYCOMMAND | MF_UNCHECKED);
        
        break;
        
    case IMAGE_Y:
        pSubMenu->CheckMenuItem(ID_MENUITEM_YUV, MF_BYCOMMAND | MF_UNCHECKED);
        pSubMenu->CheckMenuItem(ID_MENUITEM_Y, MF_BYCOMMAND | MF_CHECKED);
        pSubMenu->CheckMenuItem(ID_MENUITEM_U, MF_BYCOMMAND | MF_UNCHECKED);
        pSubMenu->CheckMenuItem(ID_MENUITEM_V, MF_BYCOMMAND | MF_UNCHECKED);

        break;
        
    case IMAGE_U:
        pSubMenu->CheckMenuItem(ID_MENUITEM_YUV, MF_BYCOMMAND | MF_UNCHECKED);
        pSubMenu->CheckMenuItem(ID_MENUITEM_Y, MF_BYCOMMAND | MF_UNCHECKED);
        pSubMenu->CheckMenuItem(ID_MENUITEM_U, MF_BYCOMMAND | MF_CHECKED);
        pSubMenu->CheckMenuItem(ID_MENUITEM_V, MF_BYCOMMAND | MF_UNCHECKED);
        
        break;
        
    case IMAGE_V:
        pSubMenu->CheckMenuItem(ID_MENUITEM_YUV, MF_BYCOMMAND | MF_UNCHECKED);
        pSubMenu->CheckMenuItem(ID_MENUITEM_Y, MF_BYCOMMAND | MF_UNCHECKED);
        pSubMenu->CheckMenuItem(ID_MENUITEM_U, MF_BYCOMMAND | MF_UNCHECKED);
        pSubMenu->CheckMenuItem(ID_MENUITEM_V, MF_BYCOMMAND | MF_CHECKED);
        
        break;
        
    default:
        
        break;
    }

    switch (pMainDlg->bEnMBInfo)
    {
    case TRUE:
        pSubMenu->CheckMenuItem(ID_MENUITEM_SHOWMBINFO, MF_BYCOMMAND | MF_CHECKED);
        
        break;
        
    case FALSE:
        pSubMenu->CheckMenuItem(ID_MENUITEM_SHOWMBINFO, MF_BYCOMMAND | MF_UNCHECKED);
		
        break;
        
    default:
        
        break;
    }
	
    switch (pMainDlg->bAttachFlag)
    {
    case TRUE:
        pSubMenu->CheckMenuItem(ID_MENUITEM_ATTACH, MF_BYCOMMAND | MF_CHECKED);
        
        break;
        
    case FALSE:
        pSubMenu->CheckMenuItem(ID_MENUITEM_ATTACH, MF_BYCOMMAND | MF_UNCHECKED);
		
        break;
        
    default:
        
        break;
    }
	
	if ((pMainDlg->u8PlayMode == VIEW_MODE) || (pMainDlg->s8ImgNum != 2) || (s8DlgIdx == 2))
	{
        pSubMenu->EnableMenuItem(ID_MENUITEM_GOSAMEFRAME, MF_BYCOMMAND | MF_GRAYED);
	}
	else
	{
        pSubMenu->EnableMenuItem(ID_MENUITEM_GOSAMEFRAME, MF_BYCOMMAND | MF_ENABLED);
	}

	if (pMainDlg->u8PlayMode == VIEW_MODE)
	{
		pSubMenu->EnableMenuItem(ID_MENUITEM_NOTICE, MF_BYCOMMAND | MF_GRAYED);
		pSubMenu->CheckMenuItem(ID_MENUITEM_NOTICE, MF_BYCOMMAND | MF_UNCHECKED);
	}
	else
	{
        pSubMenu->EnableMenuItem(ID_MENUITEM_NOTICE, MF_BYCOMMAND | MF_ENABLED);

		if (pMainDlg->bNoticeFlag == TRUE)
		{
			pSubMenu->CheckMenuItem(ID_MENUITEM_NOTICE, MF_BYCOMMAND | MF_CHECKED);
		}
		else
		{
			pSubMenu->CheckMenuItem(ID_MENUITEM_NOTICE, MF_BYCOMMAND | MF_UNCHECKED);
		}
	}

    pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, cursorPos.x, cursorPos.y, this);   //在指定位置显示弹出菜单
}

void CChildDlg::set_image_mode(uint8 u8ImageMode) 
{
    // TODO: Add your command handler code here
    CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;
    
    
    pMainDlg->u8ImageMode   = u8ImageMode;
    if (pMainDlg->get_play_status() != PLAY_STATUS)
    {
        pMainDlg->show_one_frame(FALSE);
    }
}

void CChildDlg::enable_mbinfo_dlg()
{
    CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;
    
    
	pMainDlg->bShowMBInfo	= FALSE;	//++ bEnMBInfo 状态切换之后默认不显示宏块信息
    pMainDlg->bEnMBInfo	= !pMainDlg->bEnMBInfo;
	if (pMainDlg->bEnMBInfo == FALSE)
	{
		pMainDlg->hide_MBinfo_dlg();
	}
}

void CChildDlg::get_pixel_value()
{
    uint8	u8LumaPointNumX;		//++ 亮度水平方向需要显示的点数
    uint8	u8LumaPointNumY;		//++ 亮度垂直方向需要显示的点数
    uint8	u8ChroPointNumX;		//++ 色度水平方向需要显示的点数
    uint8	u8ChroPointNumY;		//++ 色度垂直方向需要显示的点数
    int32	i;
    int32	j;
    int32	s32LumaWidth	 = s32Width;
    int32	s32ChroWidth	 = s32Width >> 1;
    uint8	*pLuma		 = pOrigYUV[0] + (s32ViewMBy * s32LumaWidth + s32ViewMBx);
    uint8	*pCb		 = pOrigYUV[1] + ((s32ViewMBy * s32ChroWidth + s32ViewMBx) >> 1);
    uint8	*pCr		 = pOrigYUV[2] + ((s32ViewMBy * s32ChroWidth + s32ViewMBx) >> 1);
    
    
    //++ 启用临界区保护
    CCriticalSection	CriticalSection(pCriticalSection);
    u8LumaPointNumX		 = min(s32Width - s32ViewMBx, 16);
    u8LumaPointNumY		 = min(s32Height - s32ViewMBy, 16);
    u8ChroPointNumX		 = (u8LumaPointNumX >> 1);
    u8ChroPointNumY		 = (u8LumaPointNumY >> 1);
    MBInfoDlg.u8LumaPointNumX	 = u8LumaPointNumX;
    MBInfoDlg.u8LumaPointNumY	 = u8LumaPointNumY;
    MBInfoDlg.u8ChroPointNumX	 = u8ChroPointNumX;
    MBInfoDlg.u8ChroPointNumY	 = u8ChroPointNumY;
    
    for (j = 1; j < 1 + u8LumaPointNumY; j ++)
    {
        for (i = 1; i < 1 + u8LumaPointNumX; i ++)
        {
            MBInfoDlg.pixelValue[j][i]	 = pLuma[(j - 1) * s32LumaWidth + (i - 1)];
        }
    }
    for (j = 18; j < 18 + u8ChroPointNumY; j ++)
    {
        for (i = 1; i < 1 + u8ChroPointNumX; i ++)
        {
            MBInfoDlg.pixelValue[j][i]	 = pCb[(j - 18) * s32ChroWidth + (i - 1)];
        }
        
        for (i = 9; i < 9 + u8ChroPointNumX; i ++)
        {
            MBInfoDlg.pixelValue[j][i]	 = pCr[(j - 18) * s32ChroWidth + (i - 9)];
        }
    }
}

void CChildDlg::draw_dash_frame(CRect &cRect)
{
    CWnd	*pDesktop		= GetDesktopWindow();
    CDC		*pdcDesktop		= pDesktop->GetWindowDC();
	
	
	pdcDesktop->DrawFocusRect(&cRect);
    pDesktop->ReleaseDC(pdcDesktop);
}

void CChildDlg::change_size(LPRECT pRect)
{
	int32	s32NewWidth;
	int32	s32NewHeight;
	int32	s32WndWidth;
	int32	s32WndHeight;
	RECT	*pRc	= (RECT*)pRect;


	newSizeRect		= CRect(pRc);
	s32NewWidth		= newSizeRect.Width();
	s32NewHeight	= newSizeRect.Height();
	s32WndWidth		= wndRect.Width();
	s32WndHeight	= wndRect.Height();

	if (s32NewHeight != s32WndHeight)
	{
		newSizeRect.right	= wndRect.left + s32WndWidth * s32NewHeight / s32WndHeight;
	}
	else
	{
		newSizeRect.bottom	= wndRect.top + s32WndHeight * s32NewWidth / s32WndWidth;
	}

	draw_dash_frame(oldSizeRect);
	draw_dash_frame(newSizeRect);
	pRc->left		= wndRect.left;
	pRc->right		= wndRect.right;
	pRc->top		= wndRect.top;
	pRc->bottom		= wndRect.bottom;
	oldSizeRect		= newSizeRect;
}

void CChildDlg::change_location(LPRECT pRect)
{
	RECT	*pRc	= (RECT*)pRect;


	newSizeRect		= CRect(pRc);
	oldSizeRect		= newSizeRect;
}

void CChildDlg::update_image()
{
	int32	s32ScreenWidth		= GetSystemMetrics(SM_CXSCREEN);
	int32	s32ScreenHeight		= GetSystemMetrics(SM_CYSCREEN) - 64;
    CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)this->pMainDlg;


    CRect	crRect(0, 0, s32Width, s32Height);
    CalcWindowRect(&crRect, CWnd::adjustOutside);
    s32ZoomWidth	 = s32Width * newSizeRect.Width() / crRect.Width();
    s32ZoomHeight	 = s32Height * newSizeRect.Width() / crRect.Width();

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

//	SetWindowPos(NULL, newSizeRect.left, newSizeRect.top, newSizeRect.Width(), newSizeRect.Height(), SWP_NOSENDCHANGING);	//prevent receiving more moving message
	draw_dash_frame(oldSizeRect);
	resize_window();
	
	s32ViewBlkX		= s32ViewMBx * s32ZoomWidth / s32Width;
	s32ViewBlkY		= s32ViewMBy * s32ZoomHeight / s32Height;
	s32ViewBlkW		= (s32ViewMBx + 16) * s32ZoomWidth / s32Width - s32ViewBlkX;
	s32ViewBlkH		= (s32ViewMBy + 16) * s32ZoomHeight / s32Height - s32ViewBlkY;
	ShowWindow(SW_SHOW);
    GetWindowRect(&wndRect);
    oldSizeRect	= wndRect;

	pMainDlg->layout_windows();
}
