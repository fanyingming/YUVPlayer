#include "stdafx.h"
#include "scale.h"


void CScale::set_size(int32 s32Width, int32 s32Height, int32 s32ZoomWidth, int32 s32ZoomHeight)
{
	int32	s32MaxSrcBlkSize;
	

	a32SrcBlkSize[0]	= s32Width * BLK_SIZE / s32ZoomWidth; 
	a32SrcBlkSize[1]	= s32Height * BLK_SIZE / s32ZoomHeight;

	s32MaxSrcBlkSize	= (a32SrcBlkSize[0] > a32SrcBlkSize[1]) ? a32SrcBlkSize[1] : a32SrcBlkSize[0];
	if (UNIFORM_SCALE == TRUE)
	{
		a32SWSubSample[0]	= 0;
		a32SWSubSample[1]	= 0;
		while((s32MaxSrcBlkSize - 1) >> 7)
		{
			s32MaxSrcBlkSize >>= 1;
			a32SWSubSample[0] ++;
			a32SWSubSample[1] ++;
		}
		
		a32SrcBlkSize[0]	= s32MaxSrcBlkSize;
		a32SrcBlkSize[1]	= s32MaxSrcBlkSize;
	}
	else
	{
		a32SWSubSample[0]	= 0;
		while((a32SrcBlkSize[0] - 1) >> 7)
		{
			a32SrcBlkSize[0] >>= 1;
			a32SWSubSample[0] ++;
		}
		
		a32SWSubSample[1]	= 0;
		while((a32SrcBlkSize[1] - 1) >> 7)
		{
			a32SrcBlkSize[1] >>= 1;
			a32SWSubSample[1] ++;
		}
	}
}

void CScale::creat_scale_table()
{
	int32	i;
	int32	s32WeightA;
	int32	s32WeightB;
	
	for (i = 0; i < BLK_SIZE; i ++)
	{
		aa32ScaleIdx[0][i]	= (i * a32SrcBlkSize[0]) / BLK_SIZE;	//++ 各点在水平方向的映射位置
		aa32ScaleIdx[1][i]	= (i * a32SrcBlkSize[1]) / BLK_SIZE;	//++ 各点在垂直方向的映射位置
		
		s32ScaleLineIdx[i]	= aa32ScaleIdx[1][i] * s32BufSizeH;	//++ 各点在内存中的行偏移距离
		
		aa32ScaleTap[0][i]	= BLK_SIZE - (i * a32SrcBlkSize[0]) % BLK_SIZE;	//++ 各参考点在水平方向的权重
		aa32ScaleTap[1][i]	= BLK_SIZE - (i * a32SrcBlkSize[1]) % BLK_SIZE;	//++ 各参考点在垂直方向的权重

		s32WeightA	= aa32ScaleTap[1][i];
		s32WeightB	= BLK_SIZE - aa32ScaleTap[1][i];
		u64ScaleTapV[i][0]	= ((uint64)(s32WeightA & 0xffff) << 48) | ((uint64)(s32WeightA & 0xffff) << 32) | ((uint64)(s32WeightA & 0xffff) << 16) | (uint64)(s32WeightA & 0xffff);
		u64ScaleTapV[i][1]	= ((uint64)(s32WeightB & 0xffff) << 48) | ((uint64)(s32WeightB & 0xffff) << 32) | ((uint64)(s32WeightB & 0xffff) << 16) | (uint64)(s32WeightB & 0xffff);

		s32WeightA	= aa32ScaleTap[0][i];
		s32WeightB	= BLK_SIZE - aa32ScaleTap[0][i];
		u16ScaleTapH[2 * i]			= (uint16)s32WeightA;
		u16ScaleTapH[2 * i + 1]		= (uint16)s32WeightB;
	}
}

void CScale::image_scale(uint8 *pSrc, uint8 *pDst)
{
	int32 s32StepH	= 1 << a32SWSubSample[0];
	int32 s32StepV	= 1 << a32SWSubSample[1];

    int32 iYBufHSize = s32BufSizeH;
    int32 iYBufVSize = s32BufSizeV;
    int32 iCBufHSize = iYBufHSize /2;
    int32 iCBufVSize = iYBufVSize /2;
    int32 iYBufSize = iYBufHSize *iYBufVSize;
    int32 iCBufSize = iCBufHSize *iCBufVSize;

    int32 iYHoSize = s32ZoomWidth * s32StepH;
    int32 iYVoSize = s32ZoomHeight * s32StepV;
    int32 iCHoSize = iYHoSize /2;
    int32 iCVoSize = iYVoSize /2;
    
    uint8 *pYSrc = pSrc;
    uint8 *pYDst = pDst;
    uint8 *pCSrc = pSrc +iYBufSize;
    uint8 *pCDst = pDst +iYBufSize;


	if ((a32SrcBlkSize[0] == BLK_SIZE) && (a32SrcBlkSize[1] == BLK_SIZE))
	{
		return;
	}

    //Y part
    {
        int32 iHBlkCnt = iYHoSize/BLK_SIZE +((iYHoSize%BLK_SIZE)? 1: 0);
        int32 iVBlkCnt = iYVoSize/BLK_SIZE +((iYVoSize%BLK_SIZE)? 1: 0);

        for (int32 j = iVBlkCnt-1; j >= 0; j--)
        {
            for (int32 i = iHBlkCnt-1; i >= 0; i--)
            {
                uint8 *pYBlkSrc = pYSrc +j*a32SrcBlkSize[1]*iYBufHSize +i*a32SrcBlkSize[0];
                uint8 *pYBlkDst = pYDst +j*BLK_SIZE*iYBufHSize +i*BLK_SIZE;
                uint32 AddrYSrc = (uint32)pYBlkSrc;
                uint32 AddrYDst = (uint32)pYBlkDst;
				
				int32 umax = (j==iVBlkCnt-1)? (iYVoSize -BLK_SIZE*j-s32StepV): BLK_SIZE-s32StepV;
				int32 vmax = (i==iHBlkCnt-1)? (iYHoSize -BLK_SIZE*i-s32StepH): BLK_SIZE-s32StepH;
				int32 iDstLineOff = umax*iYBufHSize;
				
				for (int32 u = umax; u >= 0; u-=s32StepV, iDstLineOff-=(iYBufHSize*s32StepV))
				{
					int32 iOffU = s32ScaleLineIdx[u];
					int32 iOffD = iOffU +iYBufHSize;
					
					//Tap
					//      a a'  b b'
					//  A
					//  A'
					uint64 iUTap64 = u64ScaleTapV[u][0];
					uint64 iDTap64 = u64ScaleTapV[u][1];
					uint32 AddrScaleIdx = (uint32)(&aa32ScaleIdx[0][0]);
					uint32 AddrScaleTap = (uint32)(&u16ScaleTapH[0]);
					uint32 AddrYDstPix = AddrYDst +iDstLineOff;

					//....................................................
					//...................Y25   Y24   Y23   Y22   Y21   Y20
					//                             -----------------------
					//...................Y15   Y14 | Y13   Y12   Y11   Y10
					//
					//...................Y05   Y04 | Y03   Y02   Y01   Y00
					__asm
					{
						mov             eax,            s32StepH;
						neg             eax;
						movd            mm4,            eax;
						movd            mm5,            iYBufHSize;
						movq            mm6,            iUTap64;
						movq            mm7,            iDTap64;
						mov             ecx,            vmax;
						mov             esi,            AddrYSrc;
						mov             edi,            AddrYDstPix;
						add             esi,            iOffU;
						lea             edi,            [edi+ecx];
						mov             eax,            AddrScaleTap;
						mov             ebx,            AddrScaleIdx;
						lea             eax,            [eax+ecx*4];
						lea             ebx,            [ebx+ecx*4];
UpSampleY:
						push            ecx;
						//-----------------------------------------------------
						movd            mm0,            [eax];
						movq            mm1,            mm7;
						pmullw          mm1,            mm0;                //mm1 = A'*a A'*a'
						pmullw          mm0,            mm6;                //mm0 = A *a A* a'
						//-----------------------------------------------------
						mov             ecx,            [ebx];
						movd            edx,            mm5;
						lea             ecx,            [esi+ecx];
						//-----------------------------------------------------
						punpcklbw       mm2,            [ecx];
						punpcklbw       mm3,            [ecx+edx];
						psrlw           mm2,            8;                  //mm2 = ... Y11 Y10
						psrlw           mm3,            8;                  //mm3 = ... Y01 Y00
						pmaddwd         mm0,            mm2;
						pmaddwd         mm1,            mm3;
						paddd           mm0,            mm1;                //mm0 = Yn00
						//-----------------------------------------------------
						movd            ecx,            mm0;
						movd            edx,            mm4;
						shr             ecx,            iDstBlkSizePowerx2;
						lea             eax,            [eax+edx*4];
						mov             [edi],          cl;
						lea             ebx,            [ebx+edx*4];
						pop             ecx;
						lea             edi,            [edi+edx];
						add             ecx,            edx;
						jge             UpSampleY;
						
						emms;
					}
                }
            }
        }
    }

    //C part
    {
        int32 iHBlkCnt = iCHoSize/BLK_SIZE +((iCHoSize%BLK_SIZE)? 1: 0);
        int32 iVBlkCnt = iCVoSize/BLK_SIZE +((iCVoSize%BLK_SIZE)? 1: 0);

        for (int32 j = iVBlkCnt-1; j >= 0; j--)
        {
            for (int32 i = iHBlkCnt-1; i >= 0; i--)
            {
                uint8 *pCBlkSrc = pCSrc +j*a32SrcBlkSize[1]*iCBufHSize +i*a32SrcBlkSize[0];
                uint8 *pCBlkDst = pCDst +j*BLK_SIZE*iCBufHSize +i*BLK_SIZE;
                uint32 AddrCSrc = (uint32)pCBlkSrc;
                uint32 AddrCDst = (uint32)pCBlkDst;
				
				int32 umax = (j==iVBlkCnt-1)? (iCVoSize -BLK_SIZE*j-s32StepV): BLK_SIZE-s32StepV;
				int32 vmax = (i==iHBlkCnt-1)? (iCHoSize -BLK_SIZE*i-s32StepH): BLK_SIZE-s32StepH;
				int32 iDstLineOff = umax*iCBufHSize;
				
				for (int32 u = umax; u >= 0; u-=s32StepV, iDstLineOff-=(iCBufHSize*s32StepV))
				{
					int32 iOffU = s32ScaleLineIdx[u]>>1;
					int32 iOffD = iOffU +iCBufHSize;

					//Tap
					//      a a'  b b'
					//  A
					//  A'
					uint64 iUTap64 = u64ScaleTapV[u][0];
					uint64 iDTap64 = u64ScaleTapV[u][1];
					uint32 AddrScaleIdx = (uint32)(&aa32ScaleIdx[0][0]);
					uint32 AddrScaleTap = (uint32)(&u16ScaleTapH[0]);
					uint32 AddrCDstPix = AddrCDst +iDstLineOff;

					//....................................................
					//...................C25   C24   C23   C22   C21   C20
					//                             -----------------------
					//...................C15   C14 | C13   C12   C11   C10
					//
					//...................C05   C04 | C03   C02   C01   C00
					int32 iPixStep_ = -s32StepH;

					__asm
					{
						movd            mm5,            iCBufSize;
						movq            mm6,            iUTap64;
						movq            mm7,            iDTap64;
						mov             ecx,            vmax;
						mov             esi,            AddrCSrc;
						mov             edi,            AddrCDstPix;
						add             esi,            iOffU;
						lea             edi,            [edi+ecx];
						mov             eax,            AddrScaleTap;
						mov             ebx,            AddrScaleIdx;
						lea             eax,            [eax+ecx*4];
						lea             ebx,            [ebx+ecx*4];
UpSampleC:
						push            ecx;
						//-----------------------------------------------------
						movd            mm0,            [eax];
						movq            mm1,            mm7;
						pmullw          mm1,            mm0;                //mm1 = A'*a A'*a'
						pmullw          mm0,            mm6;                //mm0 = A *a A* a'
						punpckldq       mm1,            mm1;                //mm1 = A'*a A'*a' A'*a A'*a'
						punpckldq       mm0,            mm0;                //mm0 = A *a A* a' A *a A* a'
						//-----------------------------------------------------
						mov             ecx,            [ebx];
						movd            edx,            mm5;
						lea             ecx,            [esi+ecx];
						//-----------------------------------------------------
						pxor            mm4,            mm4;
						movd            mm2,            [ecx];
						punpcklwd       mm2,            [ecx+edx];
						add             ecx,            iCBufHSize;
						movd            mm3,            [ecx];
						punpcklwd       mm3,            [ecx+edx];
						punpcklbw       mm2,            mm4;                //mm2 = V11 V10 U11 U10
						punpcklbw       mm3,            mm4;                //mm3 = V01 V00 U01 U00
						pmaddwd         mm2,            mm0;
						pmaddwd         mm3,            mm1;
						paddd           mm2,            mm3;
						psrld           mm2,            iDstBlkSizePowerx2;
						packssdw        mm2,            mm3;
						packuswb        mm2,            mm2;
						//-----------------------------------------------------
						movd            edx,            mm5;
						movd            ecx,            mm2;
						mov             [edi],          cl;
						mov             [edi+edx],      ch;
						//-----------------------------------------------------
						mov             edx,            iPixStep_;
						lea             eax,            [eax+edx*4];
						lea             ebx,            [ebx+edx*4];
						pop             ecx;
						lea             edi,            [edi+edx];
						add             ecx,            edx;
						jge             UpSampleC;
						
						emms;
					}
                }
            }
        }
    }
}
