#ifndef _MULTITHREAD_H_
#define _MULTITHREAD_H_


#include "define.h"


#ifdef _cplusplus
extern "C"
{
#endif


class CCriticalSection
{
public:
	CCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
	{
		m_lpCriticalSection	 = lpCriticalSection;
		EnterCriticalSection(m_lpCriticalSection);
	}

	~CCriticalSection()
	{
		LeaveCriticalSection(m_lpCriticalSection);
	}

	static void initial(LPCRITICAL_SECTION lpCriticalSection)
	{
		InitializeCriticalSection(lpCriticalSection);
	}

	static void delet(LPCRITICAL_SECTION lpCriticalSection)
	{
		DeleteCriticalSection(lpCriticalSection);
	}

private:
	LPCRITICAL_SECTION m_lpCriticalSection;
};


uint32 play_video(LPVOID pParam);


#ifdef _cplusplus
}
#endif


#endif