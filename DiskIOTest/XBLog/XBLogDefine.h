#ifndef __XB_LOG_DEFINE_H__
#define __XB_LOG_DEFINE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace xb_log
{

	enum LOG_LEVLE
	{
		// Ŀǰ�õȼ������������
		eLOG_DEBUG = 1,	// Release�²�����õȼ�LOG
		eLOG_INFO = 2,
		eLOG_WARNING = 3,
		eLOG_ERROR = 4,
		eLOG_CRITICAL = 5,

		eLOG_CLOSE = 255,	// ���⣺�ͷ�ָ���ļ����Ƶ��������
	};

	enum LOG_FLAG
	{
		eLOG_FLAG_NOT_CREATE_MONTH_DIR	= 0x01,		// ���Զ������·�Ŀ¼
		eLOG_FLAG_NOT_ADD_INFO			= 0x02,		// ����Ӷ������Ϣ
		eLOG_FLAG_NOT_FILE_NAME_FORMAT	= 0x04,		// �ļ������и�ʽ������������Զ����ɣ�
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