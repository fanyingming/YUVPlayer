#include <stdafx.h>
#include "multithread.h"
#include "YUVPlayerDlg.h"


uint32 play_video(LPVOID pParam)
{
	uint8	bEndLoop	 = FALSE;
	uint32	u32Timer;
	uint32	u32Interval;
	uint32	u32TimeSpan;
 	CString			csNotice;
 	CStringArray	csNoticeList;
	CImageDlg   	*pCurrImg;
	CYUVPlayerDlg	*pMainDlg	 = (CYUVPlayerDlg *)pParam;


	do 
	{
		if (pMainDlg->s8ImgNum == 0)
		{
			break;
		}
		
		u32Timer		 = GetTickCount();

		for (int32 i = 0; i < pMainDlg->s8ImgNum; i ++)
		{
			pCurrImg	 = pMainDlg->pImage[i];
			
			if (pMainDlg->bReversePlay == TRUE)
			{
				pMainDlg->u8EOFNum		 = 0;
				pCurrImg->bForwardOK	 = TRUE;
				pCurrImg->bEOFFlag		 = FALSE;

				if (pMainDlg->u8EOHNum != pMainDlg->s8ImgNum)
				{
					if ((pCurrImg->s32CurrFrameNr - 1) >= 1)
					{
						pCurrImg->pFile->Seek(-2 * pCurrImg->u32FrameSize, CFile::current);
						pCurrImg->s32CurrFrameNr --;
						SendMessage(pCurrImg->m_hWnd, WM_MYMESSAGE_1, pMainDlg->u8ImageMode, TRUE);
						
						if (pCurrImg->s32CurrFrameNr == 1)
						{
							pCurrImg->bEOHFlag		 = TRUE;
							pCurrImg->bBackwardOK	 = FALSE;
							pMainDlg->u8EOHNum ++;
							
							if (pMainDlg->u8EOHNum == pMainDlg->s8ImgNum)
							{
								bEndLoop	 = TRUE;
								csNotice	 = "所有图像已经到达首帧！";
							}
							else
							{
								csNoticeList.Add(pCurrImg->fileName + " 已经到达首帧，其他图像仍可继续播放！");
							}
						}
					}
				}
				else
				{
					bEndLoop	 = TRUE;
					csNotice	 = "所有图像已经到达首帧！";

					break;
				}
			}
			else
			{
				pMainDlg->u8EOHNum		 = 0;
				pCurrImg->bEOHFlag		 = FALSE;
				pCurrImg->bBackwardOK	 = TRUE;

				if (pMainDlg->u8EOFNum != pMainDlg->s8ImgNum)
				{
					if ((pCurrImg->s32CurrFrameNr + 1) <= pCurrImg->s32FrameNum)
					{
						pCurrImg->s32CurrFrameNr ++;
						SendMessage(pCurrImg->m_hWnd, WM_MYMESSAGE_1, pMainDlg->u8ImageMode, TRUE);
						
						if (pCurrImg->s32CurrFrameNr == pCurrImg->s32FrameNum)
						{
							pCurrImg->bForwardOK	 = FALSE;
							pCurrImg->bEOFFlag		 = TRUE;
							pMainDlg->u8EOFNum ++;
							
							if (pMainDlg->u8EOFNum == pMainDlg->s8ImgNum)
							{
								bEndLoop	  = TRUE;
								csNotice	  = "所有图像已经到达尾帧！";
							}
							else
							{
								csNoticeList.Add(pCurrImg->fileName + " 已经到达尾帧，其他图像仍可继续播放！");
							}
						}
					}
				}
				else
				{
					bEndLoop	  = TRUE;
					csNotice	  = "所有图像已经到达尾帧！";

					break;
				}
			}
		}

		if (pMainDlg->bShowDiffPic == TRUE)
		{
			SendMessage(pMainDlg->diffPic.m_hWnd, WM_MYMESSAGE_2, pMainDlg->u8ImageMode, TRUE);
		}
		
		if ((bEndLoop == FALSE) && (csNoticeList.GetSize() != 0))
		{
			for (int32 i = 0; i < csNoticeList.GetSize(); i ++)
			{
				AfxMessageBox(csNoticeList.GetAt(i), MB_ICONINFORMATION);
			}

			csNoticeList.RemoveAll();

			break;
		}

		if (pMainDlg->get_play_status() == PAUSE_STATUS)
		{
			break;
		}

		u32Interval	  = GetTickCount() - u32Timer;
		u32TimeSpan	  = (uint32)(1000 / pMainDlg->fFrameRate + 0.5);
		if(u32Interval < u32TimeSpan)
		{
			Sleep(u32TimeSpan - u32Interval); // sleep time in milliseconds
		}
	} while(!bEndLoop);

	pMainDlg->set_play_status(PAUSE_STATUS);
	if (bEndLoop == FALSE)
	{
		//++ 通知主线程播放已经停止
		PostMessage(pMainDlg->m_hWnd, WM_MYMESSAGE_0, 0, 0);
	}
	else
	{
		pMainDlg->m_play.SetWindowText("播    放");
  		pMainDlg->m_replay.SetWindowText("回    放");
		AfxMessageBox(csNotice, MB_ICONINFORMATION);
	}

	pMainDlg->m_pPlayThread	 = NULL;

	return 0;
}

