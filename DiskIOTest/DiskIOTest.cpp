// DiskIOTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <process.h>
#include <tchar.h>
#include <strsafe.h>
#include <time.h>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <thread>
#include <direct.h>
#include <stdio.h>

#include "XBLog/XBLog.h"

using namespace std;

// 创建文件目录
bool MakeDirectory(LPCTSTR szDirectory)
{
    BOOL bResult = CreateDirectory(szDirectory, NULL);
    return (bResult) ? true : false;
}

// 获取当前时间
struct tm GetTime(unsigned int& _precise)
{
	time_t _time;			
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	unsigned long long now = ft.dwHighDateTime;
	now <<= 32;
	now |= ft.dwLowDateTime;
	now /=10;
	now -=11644473600000000ULL;
	now /=1000;
	_time = now/1000;
	_precise = (unsigned int)(now%1000);
	struct tm tt = { 0 };
	localtime_s(&tt, &_time);

	return tt;
}

// 获取log文件名称
std::string GetFileName(struct tm tt, std::string strFileName, int index)
{
	char szModulePath[MAX_PATH];
	std::string strLogDir;
	if (0 != GetModuleFileNameA(nullptr, szModulePath, MAX_PATH))
	{
		strLogDir = szModulePath;
		strLogDir = strLogDir.substr(0, strLogDir.rfind('\\') + 1);
	}

	strLogDir += "log\\";
	_mkdir(strLogDir.c_str());

	bool bInitLog = XBLOG_InitLog("Instance Server - Mar  3 2021 17:38:16", "ServerLog/InsServer", "./", time(nullptr));

	std::stringstream ssFileName;
	ssFileName << strLogDir << strFileName 
		<< tt.tm_year + 1900 << tt.tm_mon << tt.tm_mday 
		<< tt.tm_hour <<tt.tm_min << tt.tm_sec << "_" << index <<".log";

	return ssFileName.str().c_str();
}

// 保存log
void MyLogSave(FILE* pFile, const char * log)
{
	unsigned int _precise = 0;
	tm tt = GetTime(_precise);
	fprintf (pFile, "%d-%02d-%02d %02d:%02d:%02d.%03d INFO: %s\n",
		 tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, _precise, log);

	std::fflush(pFile);
}

int fibonacci(int i)
{
	int result =0 ;
	if(i==1||i==2)
		return 1;
	else{
		result=result+fibonacci(i-1)+fibonacci(i-2);
	}
	return result;
}

// 获取两个时间差
double GetTimeElapse(chrono::steady_clock::time_point start, chrono::steady_clock::time_point end)
{
	auto duration=chrono::duration_cast<chrono::microseconds>(end-start);
	double cost_time=(double)(duration.count())*chrono::microseconds::period::num/chrono::microseconds::period::den;
	return (double)(duration.count())*chrono::microseconds::period::num/chrono::microseconds::period::den;
}

int main()
{
	// 启动Log
	std::cout << "Enter the number of logs per second: ";
	int nLogNumPerSecond = 0;
	scanf("%d", &nLogNumPerSecond);

	// 每50ms打印的log数
	int nLogNumPer_50_ms = nLogNumPerSecond / 20;

	unsigned int _precise = 0;
	tm tt = GetTime(_precise);

	int fbnc;
	double cost_time;
	int nLogNum = 0;
	chrono::steady_clock::time_point start, printLogTime,end;
	printLogTime = chrono::steady_clock::now();

	int fileIndex = 1;
	std::string strFileName = GetFileName(tt, "myfile", fileIndex);
	FILE * pFile;
	pFile = fopen (strFileName.c_str(),"w");
	while (true)
	{
		start=chrono::steady_clock::now();
		double dTimeElapse = GetTimeElapse(printLogTime, start);
		if (dTimeElapse > 10.f)
		{
			std::cout << "per ten second write log num: " << nLogNum << std::endl;
			printLogTime = start;
			nLogNum = 0;
		}

		fbnc=fibonacci(30);
		for (int m = 1; m < 80; m++)
		{
			std::stringstream ssFileName;
			ssFileName << "pingfile_" << m;
			std::string strPingFileName = ssFileName.str().c_str();
			for (int i = 1; i < nLogNumPer_50_ms + 1; i++)
			{
				//std::stringstream ss;
				//ss << "=============================================================================================================== test Disk IO write: " << i;

				//MyLogSave(pFile, ss.str().c_str());
				//LOGINS("=============================================================================================================== test Disk IO write: %d", i);
				XBLOG_LogAsync(strPingFileName.c_str(), xb_log::eLOG_INFO, "idNum:%u, p:%d", 1, 1);
				nLogNum++;
			}
		}
		end=chrono::steady_clock::now();
		cost_time=GetTimeElapse(start, end);
		//std::cout << "cost_time: " << cost_time << endl;

		int cost_time_ms = (int)(cost_time * 1000.f);
		if (cost_time * 1000 < 50)
		{
			//std::cout << "sleep time: " << 50 - cost_time_ms << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long>(50 - cost_time_ms)));
		}
		else
		{
			std::cout << "time out per 50 ms logNum:  " << nLogNumPer_50_ms << ", cost time: " << cost_time_ms << "ms" << std::endl;
		}

		int nFileSize = ftell(pFile);
		if (nFileSize > 1024 * 1024 * 100)
		{
			std::cout << "file size :" << nFileSize << std::endl;
			fclose(pFile);

			fileIndex++;
			if (fileIndex > 10)
			{
				fileIndex = 1;
			}
			strFileName = GetFileName(tt, "myfile", fileIndex);
			pFile = fopen (strFileName.c_str(),"w");
		}
	}
	fclose (pFile);

	std::system("pause");
}
