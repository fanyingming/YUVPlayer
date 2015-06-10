#ifndef _CHILDDLG_H_
#define _CHILDDLG_H_


#include "define.h"
#include "MBInfoDlg.h"


class CChildDlg : public CDialog
{
public:
    CChildDlg(UINT nID, CWnd* pParent /*=NULL*/);

    int8	s8DlgIdx;
	uint8	bSizeChanged;	//++ 窗口大小是否改变
    uint8   u8SampleFormat;
    int32	s32MBXNum;		//++ 一行宏块个数
    int32	s32MBYNum;		//++ 一列宏块个数
    int32	s32MBXIdx;		//++ 当前宏块所在列号（缩放前）
    int32	s32MBYIdx;		//++ 当前宏块所在行号（缩放前）
    int32	s32ViewMBx;		//++ 显示像素值的宏块的顶点坐标（缩放前）
    int32	s32ViewMBy;		//++ 显示像素值的宏块的顶点坐标（缩放前）
    int32	s32ViewBlkX;	//++ 显示像素值的宏块的顶点坐标（缩放后）
    int32	s32ViewBlkY;	//++ 显示像素值的宏块的顶点坐标（缩放后）
	int32	s32ViewBlkW;
	int32	s32ViewBlkH;
    int32	s32PrevBlkX;		//++ 前一个鼠标指向的宏块的顶点坐标（缩放后）
    int32	s32PrevBlkY;		//++ 前一个鼠标指向的宏块的顶点坐标（缩放后）
	int32	s32PrevBlkW;
	int32	s32PrevBlkH;
    int32	s32CurrMBx;		//++ 鼠标经过的宏块的顶点坐标（缩放前）
    int32	s32CurrMBy;		//++ 鼠标经过的宏块的顶点坐标（缩放前）
    int32	s32CurrBlkX;	//++ 鼠标经过的宏块的顶点坐标（缩放后）
    int32	s32CurrBlkY;	//++ 鼠标经过的宏块的顶点坐标（缩放后）
	int32	s32CurrBlkW;
	int32	s32CurrBlkH;
    int32	s32Width;		//++ 图像缩放前的宽度
    int32	s32Height;		//++ 图像缩放前的高度
    int32	s32ZoomWidth;	//++ 图像缩放后的宽度
    int32	s32ZoomHeight;	//++ 图像缩放后的高度
    uint32	u32FrameSize;
    uint32	u32ChroPicSize;
    uint32	u32LumaPicSize;
    HANDLE	hloc;
    LPBITMAPINFO	BmpInfo;
    CMBInfoDlg		MBInfoDlg;
    PCRITICAL_SECTION pCriticalSection;
    LPBYTE	pYUVBuff;
    LPBYTE	pRGBBuff;
    LPBYTE	pReadYUV[3];
    LPBYTE	pOrigYUV[3];
    LPBYTE	pMirrYUV[3];
    LPBYTE	pRotaYUV[3];
    LPBYTE	pDisplayLuma;
    LPBYTE	pDisplayChro;
    CString	fileName;
    CMenu	mouseMenu;
    CWnd	*pMainDlg;
    CMenu	*pSubMenu;
	CRect	wndRect; 
	CRect	newSizeRect; 
	CRect	oldSizeRect; 

    void	resize_window();
    void	get_pixel_value();
    void	set_bmp_parameter();
    void    enable_mbinfo_dlg();
    void    disable_mbinfo_dlg();
    void	remark_macroblock(CPaintDC *pDC);
    void	show_mouse_menu();
    void	mark_macroblock();
    void	show_macroblock_info();
    void	view_macroblock();
    void	color_space_convert(uint8 u8ImageMode);
	void	YV12_to_RGB24(uint8* pu8Y, uint8* pu8U, uint8* pu8V);
	void	YUY2_to_RGB24(uint8 *pu8RGBData, uint8 *pu8YUVData);
    void	set_image_mode(uint8 u8ImageMode);
    void	draw_dash_frame(CRect &rect);
	void	change_size(LPRECT pRect);
	void	change_location(LPRECT pRect);
	void	update_image();
    int32	malloc_memory();
    int32	show_image(CDC *pDC);
};


#endif