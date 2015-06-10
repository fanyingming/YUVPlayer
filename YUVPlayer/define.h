#ifndef _DEFINE_H_
#define _DEFINE_H_


typedef		char				int8;
typedef		int					int32;
typedef		short				int16;
typedef		unsigned char		uint8;
typedef		unsigned int		uint32;
typedef		unsigned short		uint16;
typedef		unsigned __int64	uint64;

//#define		LOGINFO

#define		TRUE                1
#define		FALSE               0

#define		ABS(X)				(((X) > 0 ) ? (X) : (-X))
#define		BIN(X)				(((X) == 0) ? 0 : 255)

//======= 本程序支持打开的最多图像个数 =======
#define		MAX_IMAGE_NUM		    10

//======= 图像尺寸名的最大长度 =======
#define		MAX_NAME_LEN		    20

//======= 函数返回值 =======
#define		FAILED_YUVPlayer		-1
#define		EOF_YUVPlayer			-2
#define		SKIP_YUVPlayer			-3
#define		SUCCEEDED_YUVPlayer		1

//======= 程序运行模式 =======
#define		COMP_MODE			0	//++ 两幅图像比较模式
#define		VIEW_MODE			1	//++ 多幅图像浏览模式

//======= 旋转模式 =======
#define		ROTATE_0			0	//++ 无旋转
#define		ROTATE_90			1	//++ 顺时针旋转90度
#define		ROTATE_180			2	//++ 旋转180度
#define		ROTATE_270			3	//++ 逆时针旋转90度

//======= 镜像模式 =======
#define		MIRROR_NONE			0	//++ 无镜像
#define		MIRROR_HORI			1	//++ 水平镜像
#define		MIRROR_VERT			2	//++ 垂直镜像
#define		MIRROR_BOTH			3	//++ 混合镜像

//======= 差值计算方式 =======
#define		BIN_MODE			0	//++ 按二值方式计算
#define		ABS_MODE			1	//++ 按绝对值方式计算

//======= 图像显示模式 =======
#define		IMAGE_YUV			0
#define		IMAGE_Y				1
#define		IMAGE_U				2
#define		IMAGE_V				3
#define		IMAGE_INVALID		4

//======= 图像采样格式 =======
#define		YUV400  			0
#define		YUV420				1
#define		YUV422				2
#define		YUV444				3

//======= 播放状态 =======
#define		STOP_STATUS			0
#define		PAUSE_STATUS		1
#define		PLAY_STATUS			2

//======= 像素值显示方式 =======
#define		OCT_MODE			0
#define		HEX_MODE			1

//======= 自定义消息 =======
#define		WM_MYMESSAGE_0		(WM_USER + 100)	//++ 通知主线程播放已经停止
#define		WM_MYMESSAGE_1		(WM_USER + 101)	//++ 请求显示图像
#define		WM_MYMESSAGE_2		(WM_USER + 102)	//++ 请求显示差值图像
#define		WM_MYMESSAGE_3		(WM_USER + 103)	//++ 图像窗口向主窗口传递键盘消息
#define		WM_MYMESSAGE_4		(WM_USER + 104)	//++ 通知主窗口重新打开当前图像后调整当前图像

//======= 键盘按键 =======
#define		VK_A				0X41	//++ 键盘 A 字母键
#define		VK_C				0X43	//++ 键盘 C 字母键
#define		VK_Z				0X5A	//++ 键盘 Z 字母键
#define		VK_R				0X52	//++ 键盘 R 字母键
#define		VK_S				0X53	//++ 键盘 S 字母键
#define		VK_0				0X30
#define		VK_9				0X39

//======= 请求停止播放类型 =======
#define		PAUSE_VALUE_0		0
#define		PAUSE_VALUE_1		1
#define		PAUSE_VALUE_2		2
#define		PAUSE_VALUE_3		3
#define		PAUSE_VALUE_4		4
#define		PAUSE_VALUE_5		5
#define		PAUSE_VALUE_6		6
#define		PAUSE_VALUE_7		7
#define		PAUSE_VALUE_8		8
#define		PAUSE_VALUE_9		9
#define		PAUSE_VALUE_10		10
#define		PAUSE_VALUE_11		11
#define		PAUSE_VALUE_12		12
#define		PAUSE_VALUE_13		13
#define		PAUSE_VALUE_14		14
#define		VALUE_INVALID		-1

#endif