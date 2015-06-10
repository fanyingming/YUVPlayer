#ifndef _COLOR_CONVERT_H_
#define _COLOR_CONVERT_H_
#ifdef _cplusplus
extern "C"
{
#endif


//ÑÕÉ«×ª»»4:2:0
bool YV12toRGB24(unsigned char* yData, unsigned char* vData, unsigned char* uData, 
				 unsigned char* pRGB24, int iWidth, int iHeight);
//²åÖµ
void Interpolation (unsigned char* lpDest, unsigned char* lpSrc, int SrcWid, int SrcHeight);


#ifdef _cplusplus
}
#endif
#endif