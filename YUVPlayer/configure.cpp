#include "stdafx.h"
#include "configure.h"
#include "YUVPlayerDlg.h"


void CConfigure::read_configure_file()
{
	int32	i;
	int32	j;
	CString		path;
	CString		sizeItem;
	CFile		*pConfigFile	 = new CFile();
	CYUVPlayerDlg	*pMainDlg		 = (CYUVPlayerDlg *)(AfxGetMainWnd());


	path	 = get_main_path();
	if(!pConfigFile->Open(path + "configure.dat", CFile::shareDenyWrite)) 
	{
		s32ItemNum	 = pMainDlg->m_frameSize.GetCount();
		for (i = 0; i < s32ItemNum; i ++)
		{
			priorityList.Add(s32ItemNum - i);
			pMainDlg->m_frameSize.GetLBText(i, sizeItem);
			frameSizeList.Add(sizeItem);
		}

		delete(pConfigFile);

		return;
	}

	//++ 读入数据个数
	pConfigFile->Read(&s32ItemNum, 4);

	if (s32ItemNum == 0)
	{
		s32ItemNum	 = pMainDlg->m_frameSize.GetCount();
		for (i = 0; i < s32ItemNum; i ++)
		{
			priorityList.Add(s32ItemNum - i);
			pMainDlg->m_frameSize.GetLBText(i, sizeItem);
			frameSizeList.Add(sizeItem);
		}
	}
	else
	{
		//++ 读入数据
		for (i = 0; i < s32ItemNum; i ++)
		{
			uint32	u32Priority;
			pConfigFile->Read(&u32Priority, 4);
			priorityList.Add(u32Priority);

			uint8	u8SizeLen;
			pConfigFile->Read(&u8SizeLen, 1);
			uint8	sizeItem[MAX_NAME_LEN];
			pConfigFile->Read(sizeItem, u8SizeLen);
			frameSizeList.Add(sizeItem);
		}

		//++ 对数据进行排序
		for (i = 0; i < s32ItemNum; i ++)
		{
			for (j = (i + 1); j < s32ItemNum; j ++)
			{
				uint32	item1	 = priorityList.GetAt(i);
				uint32	item2	 = priorityList.GetAt(j);

				if (item1 < item2)
				{
					priorityList.SetAt(i, item2);
					priorityList.SetAt(j, item1);

					sizeItem	 = frameSizeList.GetAt(i);
					frameSizeList.SetAt(i, frameSizeList.GetAt(j));
					frameSizeList.SetAt(j, sizeItem);
				}
			}
		}

		pMainDlg->m_frameSize.ResetContent();
		for (i = 0; i < s32ItemNum; i ++)
		{
			priorityList.SetAt(i, (s32ItemNum - i));
			sizeItem	 = frameSizeList.GetAt(i);
			pMainDlg->m_frameSize.AddString(sizeItem);
		}
	}

	pConfigFile->Close();
}

void CConfigure::write_configure_file()
{
	int32		i;
	CString		path;
	CString		item;
	CFile		*pConfigFile	 = new CFile();


	path	 = get_main_path();
	if(!pConfigFile->Open(path + "configure.dat", CFile::modeCreate | CFile::modeWrite)) 
	{
		AfxMessageBox("写配置文件失败，请确认配置文件未被使用！", MB_ICONERROR);
		delete(pConfigFile);

		return;
	}

	//++ 写入数据个数
	pConfigFile->Write(&s32ItemNum, 4);
	for (i = 0; i < s32ItemNum; i ++)
	{
		//++ 写入数据
		uint32	u32Priority	 = priorityList.GetAt(i);
		pConfigFile->Write(&u32Priority, 4);
		
		CString	sizeItem	 = frameSizeList.GetAt(i);
		uint8	u8SizeLen	 = sizeItem.GetLength() + 1;
		pConfigFile->Write(&u8SizeLen, 1);
		pConfigFile->Write((LPCTSTR)sizeItem, u8SizeLen);
	}
	
	pConfigFile->Close();
}

CString CConfigure::get_main_path()
{
	int8	s8Path[MAX_PATH];
	int32	s32Pos;


	::GetModuleFileName(NULL, s8Path, MAX_PATH);
	
	CString	path(s8Path);
	s32Pos	 = path.ReverseFind('\\');
	return path.Left(s32Pos + 1);
}

int32 CConfigure::get_size_num()
{
	return	s32ItemNum;
}

CString CConfigure::get_framesize(int32 s32ItemIdx)
{
	return frameSizeList.GetAt(s32ItemIdx);;
}

uint32 CConfigure::get_priority(int32 s32ItemIdx)
{
	return priorityList.GetAt(s32ItemIdx);
}

void CConfigure::modify_priority(int32 s32ItemIdx)
{
	int32	priority;

	priority	 = priorityList.GetAt(s32ItemIdx) + 2;
	priorityList.SetAt(s32ItemIdx, priority);
}

void CConfigure::insert_item(int32 s32Idx, int32 s32Priority, CString frameSize)
{
	s32ItemNum ++;

	priorityList.InsertAt(s32Idx, s32Priority);
	frameSizeList.InsertAt(s32Idx, frameSize);
}

void CConfigure::append_item(int32 s32Priority, CString& frameSize)
{
	priorityList.InsertAt(s32ItemNum, s32Priority);
	frameSizeList.InsertAt(s32ItemNum, frameSize);

	s32ItemNum ++;
}

void CConfigure::reset_list()
{
	s32ItemNum	 = 0;

	priorityList.RemoveAll();
	frameSizeList.RemoveAll();
}
