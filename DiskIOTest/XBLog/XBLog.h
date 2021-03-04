#ifndef __XBIRD_LOG_H__
#define __XBIRD_LOG_H__

#include <XBLog/XBLogDefine.h>

#if defined(WIN32) && !defined(_XB_CLIENT_)
#ifdef XB_LOG_EXPORTS
#define XB_LOG_API __declspec(dllexport)
#else
#define XB_LOG_API __declspec(dllimport)
#endif
#else
#ifndef XB_LOG_API
#define XB_LOG_API
#endif
#endif

#ifndef _XB_CLIENT_

#include <XBLog/XBLogServer.h>

// 接口日志宏
#define XBLOG_InitLog			xb_log::InitLog
#define XBLOG_LogSync			xb_log::LogSync
#define XBLOG_LogMainLogSync	xb_log::LogMainLogSync
#define XBLOG_LogAsync			xb_log::LogAsync
#define XBLOG_LogSpecialAsync	xb_log::LogSpecialAsync
#define XBLOG_LogMainLogAsync	xb_log::LogMainLogAsync
#define XBLOG_DirectLog			xb_log::DirectLog
#define XBLOG_MyLogSave			xb_log::MyLogSave
#define XBLOG_CloseXBLog		xb_log::CloseXBLog
#define XBLOG_CloseFile			xb_log::CloseFile


// 定制化日志宏
#define LOGWARNING		XBLog_MainLogWARNING
#define LOGMSG			XBLog_MainLogMessage
#define LOGERROR		XBLog_MainLogERROR
#define LOGMAINDEBUG	XBLog_MainLogMAIN_DEBUG

#define LOGCATCH		XBLog_InlineLogSyncCATCH_ERROR
#define LOGSTAMPCHK		XBLog_InlineLogSTAMP_CHECK
#define LOGCHEAT		XBLog_InlineLogCHEAT
#define LOGDBCRASH		XBLog_InlineLogDBCRASH
#define LOGACTIONERROR  XBLog_InlineLogACTION_ERROR
#define LOGSTRERROR     XBLog_InlineLogSTRERROR
#define LOGNETWORK		XBLog_InlineLogNETWORK
#define	LOGPS			XBLog_InlineLogLOGPS
#define	LOGINS			XBLog_InlineLogLOGINS
#define	LOGDEBUG		XBLog_InlineLogDEBUG
#define LOGSTACKINFO	XBLog_InlineLogSTACKINFO
#define LOGUAI			XBLog_InlineLogUAI
#define LOGMYSAVE		xb_log::MyLogSave

#else

// 接口日志宏
#define XBLOG_InitLog			
#define XBLOG_LogSync			
#define XBLOG_LogMainLogSync	
#define XBLOG_LogAsync			
#define XBLOG_LogSpecialAsync	
#define XBLOG_LogMainLogAsync	
#define XBLOG_DirectLog			
#define XBLOG_MyLogSave			
#define XBLOG_CloseXBLog		
#define XBLOG_CloseFile			


// 定制化日志宏
#define LOGWARNING		printf
#define LOGMSG			printf
#define LOGERROR		printf
#define LOGMAINDEBUG	printf

#define LOGCATCH		printf
#define LOGSTAMPCHK		printf
#define LOGCHEAT		printf
#define LOGDBCRASH		printf
#define LOGACTIONERROR  printf
#define LOGSTRERROR     printf
#define LOGNETWORK		printf
#define	LOGPS			printf
#define	LOGINS			printf
#define	LOGDEBUG		printf
#define LOGSTACKINFO	printf
#define LOGMYSAVE		printf

#endif


#endif