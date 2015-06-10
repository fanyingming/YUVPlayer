#ifndef _YUVRGV24_H_
#define _YUVRGV24_H_
#ifdef _cplusplus
extern "C"
{
#endif


//ÑÕÉ«×ª»»4:2:0
void ConvertYUVtoRGB(unsigned char* yData, unsigned char* vData, unsigned char* uData, 
				 unsigned char* pRGB24, int iWidth, int iHeight);

#ifdef _cplusplus
}
#endif
#endif