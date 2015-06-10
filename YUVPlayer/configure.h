#ifndef _CONFIGURE_H_
#define _CONFIGURE_H_


#include "define.h"


class CConfigure
{
public:
	void		reset_list();
	void		write_configure_file();
	void		insert_item(int32 s32Idx, int32 s32Priority, CString frameSize);
	void		append_item(int32 s32Priority, CString& frameSize);
	void		modify_priority(int32 s32ItemIdx);
	void		read_configure_file();
	int32		get_size_num();
	uint32		get_priority(int32 s32ItemIdx);
	CString		get_framesize(int32 s32ItemIdx);
	
private:
	int32			s32ItemNum;		//++ 图像尺寸类型个数
	CUIntArray		priorityList;	//++ 优先级列表
	CStringArray	frameSizeList;	//++ 尺寸列表

	CString		get_main_path();
};


#endif