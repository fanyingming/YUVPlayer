#ifndef _LOG_H_
#define _LOG_H_


#include "define.h"


class CLog
{
public:
	CLog(CString csLogInfo)
	{
		CString	csLogFile	 = "c:/log.txt";

		write_log_info(csLogFile, csLogInfo);
	}
	
	CLog(CString csLogFile, CString csLogInfo)
	{
		write_log_info(csLogFile, csLogInfo);	
	}
	
	void write_log_info(CString &csLogFile, CString &csLogInfo)
	{
		try
		{
			CFile	cfLogFile;
			CTime	ctTime = CTime::GetCurrentTime();
			CString csTemp = ctTime.Format("<%Y-%m-%d %H:%M:%S>: ");

			if (cfLogFile.Open(csLogFile, CFile::modeWrite|CFile::modeNoTruncate|CFile::modeCreate|CFile::typeBinary))
			{
				int32 s32DataLen;

				csLogInfo	 = csTemp + csLogInfo + "\r\n";
				s32DataLen	 = csLogInfo.GetLength();
				cfLogFile.SeekToEnd();
				cfLogFile.Write(csLogInfo.GetBuffer(s32DataLen), s32DataLen);
				cfLogFile.Close();
			}
		}
		catch (...)
		{
			AfxMessageBox("日志模块异常!");
		}		
	}
};

#endif