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
// Author：ycf
// 使用说明：
//		(1)使用前需要调用 InitLog()
//		(2)所有接口的FileName传入的标准格式：目录/文件名, eg:ServrLog/TestLog(不要使用"\")，如果只包含文件名则自动添加主日志文件的目录。
//		(3)该日志系统有一个主日志文件的概念，使用时应将程序运行过程中需要特别注意的信息输出至该文件，
//			以下宏LOGMSG（信息）、LOGWARNING（警告）、LOGERROR（错误）、LOGMAINDEBUG（调试），输出的目标日志是主日志文件
//		(4)含有格式化字符串功能的接口比不含该功能的接口消耗更大。
//		(5)对于常用的目录可以考虑自定义宏，参考LOGNETWORK定义方式
// 备注：该库设计与实现上，性能并不是放在第一位，在性能牺牲不大的情况下，优先考虑使用过程的便捷性，例如格式化字符串以及文件路径创建
////////////////////////////////////////////////////////////////////////////////////

namespace xb_log
{
	// 初始化
	// param:
	//	@pTitle：用于日志输出抬头（目前还未支持）
	//	@pFilename：系统的主Log文件（标准格式：目录/文件名 eg:ServrLog/TestLog）
	//	@tStart：程序开始时间
	extern XB_LOG_API bool InitLog(const char * pTitle, const char * pFilename, const char * baseDir, time_t tStart);
	// 同步log
	extern XB_LOG_API void LogSync(const char * pFilename, const char * pBuf, unsigned char ucLevel = eLOG_INFO);
	// 同步log，包含格式化字符串功能
	extern XB_LOG_API void LogSync(const char * pFilename, unsigned char ucLevel, const char * sFormat, ...);
	// 同步步log-写主文件
	extern XB_LOG_API void LogMainLogSync(const char * pBuf, unsigned char ucLevel = eLOG_INFO);
	// 同步步log-写主文件
	extern XB_LOG_API void LogMainLogSync(unsigned char ucLevel, const char * sFormat, ...);
	// 异步log
	extern XB_LOG_API void LogAsync(const char * pFilename, const char * pBuf, unsigned char ucLevel = eLOG_INFO);
	// 异步log，包含格式化字符串功能
	extern XB_LOG_API void LogAsync(const char * pFilename, unsigned char ucLevel, const char * sFormat, ...);
	// 异步log-写主文件
	extern XB_LOG_API void LogMainLogAsync(const char * pBuf, unsigned char ucLevel = eLOG_INFO);
	// 异步special log，包含格式化字符串功能
	extern XB_LOG_API void LogSpecialAsync(const char * pFilename, unsigned char ucLevel, const char * sFormat, ...);
	// 异步定制化的日志接口
	// param:	bNotMonthDir:是否需要自动创建月份路径
	//			bNotAddInfo：是否需要添加标签等额外信息
	extern XB_LOG_API void DirectLog(const char * pFilename, const char * pBuf, bool bNotMonthDir, bool bNotAddInfo);
	// 异步Log-原有接口
	extern XB_LOG_API void MyLogSave(const char *pFilename, const char *sFormat, ...);
	// 关闭XBLog，将异步Log全部写入到文件
	extern XB_LOG_API void CloseXBLog();
	// 关闭指定文件
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
extern char		log_szFilename[];		// 内部使用
extern char		log_szDBLog[];			// 内部使用

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