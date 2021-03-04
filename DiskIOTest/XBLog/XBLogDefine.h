#ifndef __XB_LOG_DEFINE_H__
#define __XB_LOG_DEFINE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace xb_log
{

	enum LOG_LEVLE
	{
		// 目前该等级不可随意添加
		eLOG_DEBUG = 1,	// Release下不输出该等级LOG
		eLOG_INFO = 2,
		eLOG_WARNING = 3,
		eLOG_ERROR = 4,
		eLOG_CRITICAL = 5,

		eLOG_CLOSE = 255,	// 特殊：释放指定文件名称的相关数据
	};

	enum LOG_FLAG
	{
		eLOG_FLAG_NOT_CREATE_MONTH_DIR	= 0x01,		// 不自动创建月份目录
		eLOG_FLAG_NOT_ADD_INFO			= 0x02,		// 不添加额外的信息
		eLOG_FLAG_NOT_FILE_NAME_FORMAT	= 0x04,		// 文件不进行格式化（添加日期自动生成）
	};
	

	#define		BUFSIZE		1024
	#define		LOG_BUFFER_SIZE 1152
	#define		FILE_DIR_SIZE	64

	typedef struct _ST_XB_LOG_INFO
	{
		char szFileDir[FILE_DIR_SIZE];
		char szLogBuffer[LOG_BUFFER_SIZE];
		unsigned char ucLevel;
		unsigned char ucFlag;
	}ST_XB_LOG_INFO, *PST_XB_LOG_INFO;

}



#endif