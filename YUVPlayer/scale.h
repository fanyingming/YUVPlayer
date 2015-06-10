#ifndef _SCALE_H_
#define _SCALE_H_


#include "define.h"


#define		BLK_SIZE				128
#define		iDstBlkSizePower		7
#define		iDstBlkSizePowerx2		iDstBlkSizePower * 2

#define		UNIFORM_SCALE	FALSE


class CScale
{
public:
	void set_size(int32 s32Width, int32 s32Height, int32 s32ZoomWidth, int32 s32ZoomHeight);
	void creat_scale_table();
	void image_scale(uint8 *pSrc, uint8 *pDst);

private:
	uint16	u16ScaleTapH[BLK_SIZE * 2];
	int32	s32ZoomWidth;
	int32	s32ZoomHeight;
	int32	s32BufSizeH;
	int32	s32BufSizeV;
	int32	s32ScaleLineIdx[BLK_SIZE];
	int32	aa32ScaleTap[2][BLK_SIZE];
	int32	aa32ScaleIdx[2][BLK_SIZE];
	int32	a32SrcBlkSize[2];
	int32	a32SWSubSample[2];
	uint64	u64ScaleTapV[BLK_SIZE][2];
};


#endif