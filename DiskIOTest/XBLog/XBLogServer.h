#ifndef __XBIRD_LOG_SERVER_H__
#define __XBIRD_LOG_SERVER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(WIN32) && !defined(_XB_CLIENT_)
#ifdef XB_LOG_EXPORTS
#define XB_LOG_API __declspec(dllexport)
#else
#define XB_LOG_API __declspec(dllimport)

#if (defined _DEBUG || defined DEBUG)
#pragma comment(lib, "XBLog.lib")
#else
#pragma comment(lib, "XBLog.lib")
#endif
#endif
#else
#ifndef XB_LOG_API
#define XB_LOG_API
#endif
#endif

#include <cstdarg>
#include <cstdio>
#include <ctime>
#ifdef WIN32
#include <WTypes.h>
#include "mgs_framework/BaseCommon.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>
#endif

////////////////////////////////////////////////////////////////////////////////////
// Author��ycf
// ʹ��˵����
//		(1)ʹ��ǰ��Ҫ���� InitLog()
//		(2)���нӿڵ�FileName����ı�׼��ʽ��Ŀ¼/�ļ���, eg:ServrLog/TestLog(��Ҫʹ��"\")�����ֻ�����ļ������Զ��������־�ļ���Ŀ¼��
//		(3)����־ϵͳ��һ������־�ļ��ĸ��ʹ��ʱӦ���������й�������Ҫ�ر�ע�����Ϣ��������ļ���
//			���º�LOGMSG����Ϣ����LOGWARNING�����棩��LOGERROR�����󣩡�LOGMAINDEBUG�����ԣ��������Ŀ����־������־�ļ�
//		(4)���и�ʽ���ַ������ܵĽӿڱȲ����ù��ܵĽӿ����ĸ���
//		(5)���ڳ��õ�Ŀ¼���Կ����Զ���꣬�ο�LOGNETWORK���巽ʽ
// ��ע���ÿ������ʵ���ϣ����ܲ����Ƿ��ڵ�һλ���������������������£����ȿ���ʹ�ù��̵ı���ԣ������ʽ���ַ����Լ��ļ�·������
////////////////////////////////////////////////////////////////////////////////////

namespace xb_log
{
	// ��ʼ��
	// param:
	//	@pTitle��������־���̧ͷ��Ŀǰ��δ֧�֣�
	//	@pFilename��ϵͳ����Log�ļ�����׼��ʽ��Ŀ¼/�ļ��� eg:ServrLog/TestLog��
	//	@tStart������ʼʱ��
	extern XB_LOG_API bool InitLog(const char * pTitle, const char * pFilename, const char * baseDir, time_t tStart);
	// ͬ��log
	extern XB_LOG_API void LogSync(const char * pFilename, const char * pBuf, unsigned char ucLevel = eLOG_INFO);
	// ͬ��log��������ʽ���ַ�������
	extern XB_LOG_API void LogSync(const char * pFilename, unsigned char ucLevel, const char * sFormat, ...);
	// ͬ����log-д���ļ�
	extern XB_LOG_API void LogMainLogSync(const char * pBuf, unsigned char ucLevel = eLOG_INFO);
	// ͬ����log-д���ļ�
	extern XB_LOG_API void LogMainLogSync(unsigned char ucLevel, const char * sFormat, ...);
	// �첽log
	extern XB_LOG_API void LogAsync(const char * pFilename, const char * pBuf, unsigned char ucLevel = eLOG_INFO);
	// �첽log��������ʽ���ַ�������
	extern XB_LOG_API void LogAsync(const char * pFilename, unsigned char ucLevel, const char * sFormat, ...);
	// �첽log-д���ļ�
	extern XB_LOG_API void LogMainLogAsync(const char * pBuf, unsigned char ucLevel = eLOG_INFO);
	// �첽special log��������ʽ���ַ�������
	extern XB_LOG_API void LogSpecialAsync(const char * pFilename, unsigned char ucLevel, const char * sFormat, ...);
	// �첽���ƻ�����־�ӿ�
	// param:	bNotMonthDir:�Ƿ���Ҫ�Զ������·�·��
	//			bNotAddInfo���Ƿ���Ҫ��ӱ�ǩ�ȶ�����Ϣ
	extern XB_LOG_API void DirectLog(const char * pFilename, const char * pBuf, bool bNotMonthDir, bool bNotAddInfo);
	// �첽Log-ԭ�нӿ�
	extern XB_LOG_API void MyLogSave(const char *pFilename, const char *sFormat, ...);
	// �ر�XBLog�����첽Logȫ��д�뵽�ļ�
	extern XB_LOG_API void CloseXBLog();
	// �ر�ָ���ļ�
	extern XB_LOG_API void CloseFile(const char * pFilename);
}

#ifdef WIN32
#define XBLog_MainLog(x,y)\
extern XB_LOG_API inline void	XBLog_MainLog##y(const char * sFormat, ...)\
{\
 	int		ret = 0;\
 	int		head_len = 0;\
 	va_list argptr;\
 	SYSTEMTIME sys;\
 	::GetLocalTime(&sys);\
 	DWORD dwThreadId = GetCurrentThreadId();\
 	char	buf[1024] = "";\
 	head_len = _snprintf_s(buf, sizeof(buf), sizeof(buf), "%02d:%02d:%02d.%03d %4u ", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, dwThreadId);\
 	if (head_len < 0) return;\
 	va_start( argptr, sFormat);	\
 	ret = vsnprintf_s(buf + head_len, sizeof(buf) - head_len - 1, sizeof(buf) - head_len - 1, sFormat, argptr);\
 	va_end( argptr );	\
 	if ( ret >= 0 && ret <= (int)(sizeof(buf)) - head_len - 1) {\
 		xb_log::LogMainLogAsync(buf, x); }\
				else {	\
		xb_log::LogMainLogSync("ASSERT(ret < 1024-2)", xb_log::eLOG_ERROR); }\
}

#define XBLog_InlineLog(x,y)\
extern XB_LOG_API inline void	XBLog_InlineLog##y(const char * sFormat, ...)\
{\
	int		ret = 0;\
	int		head_len = 0;\
	va_list argptr;\
	SYSTEMTIME sys;\
	::GetLocalTime(&sys);\
	DWORD dwThreadId = GetCurrentThreadId();\
	char	buf[1024] = "";\
	head_len = _snprintf_s(buf, sizeof(buf), sizeof(buf), "%02d:%02d:%02d.%03d %4u ", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, dwThreadId);\
	if (head_len < 0) return;\
	va_start( argptr, sFormat);\
	ret = vsnprintf_s(buf + head_len, sizeof(buf) - head_len - 1, sizeof(buf) - head_len - 1, sFormat, argptr);\
	va_end( argptr );\
	if ( ret >= 0 && ret <= (int)(sizeof(buf)) - head_len - 1) {\
		xb_log::LogAsync(x, buf, xb_log::eLOG_INFO); }\
						else {\
		xb_log::LogMainLogSync("ASSERT(ret < 1024-2)", xb_log::eLOG_ERROR); }\
}

#define XBLog_InlineLogSync(x,y)\
extern XB_LOG_API inline void	XBLog_InlineLogSync##y(const char * sFormat, ...)\
{\
	int		ret = 0;\
	int		head_len = 0;\
	va_list argptr;\
	SYSTEMTIME sys;\
	::GetLocalTime(&sys);\
	DWORD dwThreadId = GetCurrentThreadId();\
	char	buf[1024] = "";\
	head_len = _snprintf_s(buf, sizeof(buf), sizeof(buf), "%02d:%02d:%02d.%03d %4u ", sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, dwThreadId);\
	if (head_len < 0) return;\
	va_start( argptr, sFormat);\
	ret = vsnprintf_s(buf + head_len, sizeof(buf) - head_len - 1, sizeof(buf) - head_len - 1, sFormat, argptr);\
	va_end( argptr );\
	if ( ret >= 0 && ret <= (int)(sizeof(buf)) - head_len - 1) {\
		xb_log::LogSync(x, buf, xb_log::eLOG_INFO); }\
																								else {\
		xb_log::LogMainLogSync("ASSERT(ret < 1024-2)", xb_log::eLOG_ERROR); }\
}
#else
#define XBLog_MainLog(x,y)\
extern XB_LOG_API inline void	XBLog_MainLog##y(const char * sFormat, ...)\
{\
 	int		ret = 0;\
 	int		head_len = 0;\
 	char	buf[1024] = "";\
	struct tm *t;\
	time_t tt;\
	time(&tt);\
	t = localtime(&tt);\
 	head_len = snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%03d", t->tm_hour, t->tm_min, t->tm_sec, 0);\
 	if (head_len < 0) return;\
	va_list argptr;\
 	va_start( argptr, sFormat);	\
 	ret = vsnprintf(buf + head_len, sizeof(buf) - head_len - 1, sFormat, argptr);\
 	va_end( argptr );	\
 	if ( ret >= 0 && ret <= (int)(sizeof(buf)) - head_len - 1) {\
 		xb_log::LogMainLogAsync(buf, x); }\
								else {	\
		xb_log::LogMainLogSync("ASSERT(ret < 1024-2)", xb_log::eLOG_ERROR); }\
}

#define XBLog_InlineLog(x,y)\
extern XB_LOG_API inline void	XBLog_InlineLog##y(const char * sFormat, ...)\
{\
 	int		ret = 0;\
 	int		head_len = 0;\
 	char	buf[1024] = "";\
 	struct tm *t;\
	time_t tt;\
	time(&tt);\
	t = localtime(&tt);\
 	head_len = snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%03d", t->tm_hour, t->tm_min, t->tm_sec, 0);\
 	if (head_len < 0) return;\
	va_list argptr;\
	va_start( argptr, sFormat);\
	ret = vsnprintf(buf + head_len, sizeof(buf) - head_len - 1, sFormat, argptr);\
	va_end( argptr );\
	if ( ret >= 0 && ret <= (int)(sizeof(buf)) - head_len - 1) {\
		xb_log::LogAsync(x, buf, xb_log::eLOG_INFO); }\
												else {\
		xb_log::LogMainLogSync("ASSERT(ret < 1024-2)", xb_log::eLOG_ERROR); }\
}

#define XBLog_InlineLogSync(x,y)\
extern XB_LOG_API inline void	XBLog_InlineLogSync##y(const char * sFormat, ...)\
{\
 	int		ret = 0;\
 	int		head_len = 0;\
 	char	buf[1024] = "";\
 	struct tm *t;\
	time_t tt;\
	time(&tt);\
	t = localtime(&tt);\
 	head_len = snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%03d", t->tm_hour, t->tm_min, t->tm_sec, 0);\
 	if (head_len < 0) return;\
	va_list argptr;\
	va_start( argptr, sFormat);\
	ret = vsnprintf(buf + head_len, sizeof(buf) - head_len - 1, sFormat, argptr);\
	va_end( argptr );\
	if ( ret >= 0 && ret <= (int)(sizeof(buf)) - head_len - 1) {\
		xb_log::LogSync(x, buf, xb_log::eLOG_INFO); }\
																																																else {\
		xb_log::LogMainLogSync("ASSERT(ret < 1024-2)", xb_log::eLOG_ERROR); }\
}
#endif
/////////////////////////////////////////////////////////////////
extern char		log_szFilename[];		// �ڲ�ʹ��
extern char		log_szDBLog[];			// �ڲ�ʹ��

XBLog_MainLog(xb_log::eLOG_INFO, Message)
XBLog_MainLog(xb_log::eLOG_WARNING, WARNING)
XBLog_MainLog(xb_log::eLOG_ERROR, ERROR)
XBLog_MainLog(xb_log::eLOG_DEBUG, MAIN_DEBUG)

XBLog_InlineLogSync("ServerLog/catch", CATCH_ERROR)
XBLog_InlineLog("ServerLog/actionerror", ACTION_ERROR)
XBLog_InlineLog("ServerLog/stampchk", STAMP_CHECK)
XBLog_InlineLog("ServerLog/dump", DUMP)
XBLog_InlineLog("ServerLog/cheat", CHEAT)
XBLog_InlineLog("ServerLog/strerror", STRERROR)
XBLog_InlineLog("ServerLog/dbcrash", DBCRASH)
XBLog_InlineLog("ServerLog/network", NETWORK)
XBLog_InlineLog("ServerLog/platform_info", LOGPS)
XBLog_InlineLog("ServerLog/InsInfo", LOGINS)
XBLog_InlineLog("ServerLog/DebugInfo", DEBUG)
XBLog_InlineLog("ServerLog/StackInfo", STACKINFO)
XBLog_InlineLog("ServerLog/UtilAI", UAI)

#endif