#include <iostream>
#include <time.h>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <thread>
#include <stdio.h>
#include <vector>

#ifdef WIN32
#include <Windows.h>
#include <process.h>
#include <tchar.h>
#include <strsafe.h>
#include <direct.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <wchar.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#endif

#include <fstream>
#include <filesystem>

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

// g++ -std=c++17 DiskIOTest.cpp -o diskIOTest -lstdc++fs -g

using namespace std;

// 创建文件目录
bool MakeDirectory(std::string szDirectory)
{
#ifdef WIN32
    BOOL bResult = CreateDirectoryA(szDirectory.c_str(), NULL);
    return (bResult) ? true : false;
#else
	return (mkdir(szDirectory.c_str(), 0755) == 0);
#endif
}

long long clib_time()
{
	time_t result = 0;
	::time(&result);
	return result;
}

extern tm time2tm( time_t t )
{
#ifdef WIN32
	struct tm tt = { 0 };
	localtime_s(&tt, &t);
	return tt;
#else
	struct tm tt = { 0 };
	localtime_r(&t, &tt);
	return tt;
#endif
}

// 获取当前时间
struct tm GetTime(unsigned int& _precise)
{
	long long curTime = clib_time();
	return time2tm(curTime);
}


// 获取log文件名称
std::string GetFileName(struct tm tt, std::string strFileName, int fileIndex, int logIndex)
{
	std::string strLogDir = std::filesystem::current_path().generic_string();
#ifdef WIN32
	strLogDir = strLogDir.substr(0, strLogDir.rfind('\\') + 1);
	strLogDir += "log\\";
#else
	strLogDir += "/log/";
#endif

	MakeDirectory(strLogDir.c_str());

	std::stringstream ssFileName;
	ssFileName << strLogDir << strFileName 
		<< tt.tm_year + 1900 << tt.tm_mon << tt.tm_mday 
		<< tt.tm_hour <<tt.tm_min << tt.tm_sec << "_"<< fileIndex << "_" << logIndex <<".log";

	return ssFileName.str().c_str();
}

// 保存log
void MyLogSave(FILE* pFile, const char * log)
{
	unsigned int _precise = 0;
	tm tt = GetTime(_precise);
	fprintf (pFile, "%d-%02d-%02d %02d:%02d:%02d.%03d INFO: %s\n",
		 tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, _precise, log);

	//std::fflush(pFile);
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
	// double cost_time=(double)(duration.count())*chrono::microseconds::period::num/chrono::microseconds::period::den;
	return (double)(duration.count())*chrono::microseconds::period::num/chrono::microseconds::period::den;
}

// 打印当前时间
void PrintNow()
{
	auto nTime = chrono::system_clock::now().time_since_epoch();
	// 输出当前时间: 秒
	std::cout << "time now seconds     : " << chrono::duration_cast<chrono::seconds>(nTime).count() << std::endl;
	// 输出当前时间: 毫秒
	std::cout << "time now milliseconds: " << chrono::duration_cast<chrono::milliseconds>(nTime).count() << std::endl;
	// 输出当前时间: 微秒
	std::cout << "time now microseconds: " << chrono::duration_cast<chrono::microseconds>(nTime).count() << std::endl;
	// 输出当前时间: 纳秒
	std::cout << "time now nanoseconds : " << nTime.count() << std::endl;
	fflush(stdout);

	/*
	 * 以上输出的结果：
		time now seconds     : 1640245490
		time now milliseconds: 1640245490771
		time now microseconds: 1640245490771712
		time now nanoseconds : 1640245490771712012
	 * */

	// 打印当前时间字符串，格式为： Thu Dec 23 14:54:06 2021
	std::time_t tt;
	tt = chrono::system_clock::to_time_t(chrono::system_clock::now());
	std::cout << "today is: " << ctime(&tt);

	chrono::steady_clock::time_point start, end;
	start = chrono::steady_clock::now();
	end = chrono::steady_clock::now();
	double dTimeElapse = GetTimeElapse(start, end);
	std::cout << "dTimeElapse: " << dTimeElapse;

}

//  磁盘IO测试
//  nlogFileNum: 测试写入的文件数
//  logNumPerTime: 每个文件每次写入的log条数
//  logInterval: 写入的时间间隔
void DiskIOTest(int nlogFileNum, int logNumPerTime, int logInterval)
{
	unsigned int _precise = 0;
	tm tt = GetTime(_precise);

	int fbnc;
	double cost_time;
	int nLogNum = 0;
	chrono::steady_clock::time_point start, printLogTime, end;
	printLogTime = chrono::steady_clock::now();

	// 初始化打开需要打印log的文件
	int fileIndex = 1;
	std::vector<FILE*> vecFILE;
	for (int i = 0; i < nlogFileNum; i++)
	{
		std::string strFileName = GetFileName(tt, "myfile", i, fileIndex);
		FILE * pFile;
		pFile = fopen (strFileName.c_str(),"w");
		if (pFile != nullptr)
		{
			vecFILE.push_back(pFile);
		}
	}

	// 每个文件根据时间间隔打印log
	while (true)
	{
		// 获取起始时间
		start=chrono::steady_clock::now();
		double dTimeElapse = GetTimeElapse(printLogTime, start);
		if (dTimeElapse > 10.f)
		{
			// 每10秒输出写入的log数
			std::cout << "per ten second write log num: " << nLogNum << std::endl;
			printLogTime = start;
			nLogNum = 0;
		}

		fbnc=fibonacci(30);
		bool isFileSizeFull = false;
		for (int i = 1; i < logNumPerTime + 1; i++)
		{
			std::stringstream ss;
			ss << "=============================================================================================================== test Disk IO write: " << i;

			for (int j = 0; j < vecFILE.size(); j++)
			{
				FILE* pFile = vecFILE[j];
				if (pFile != nullptr)
				{
					MyLogSave(pFile, ss.str().c_str());
					nLogNum++;
				}
			}
		}

		// 获取结束时间
		end=chrono::steady_clock::now();

		// 计算时间间隔内打印完log后剩余多少时间，并进行sleep
		cost_time=GetTimeElapse(start, end);
		int cost_time_ms = (int)(cost_time * 1000.f);
		if (cost_time_ms < logInterval)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long>(logInterval - cost_time_ms)));
		}
		else
		{
			std::cout << "time out per "<< cost_time_ms <<" ms logNum:  " << logNumPerTime << ", cost time: " << cost_time_ms << "ms" << std::endl;
		}
	}// end of while

	// 关闭文件
	for (int i = 0; i < vecFILE.size(); i++)
	{
		FILE* pFile = vecFILE[i];
		if (pFile != nullptr)
		{
			fclose(pFile);
		}
	}
}

int main()
{
	PrintNow();

	std::cout << "Enter the number of log files: ";
	int nLogFile = 0;
	scanf("%d", &nLogFile);

	std::cout << "Enter the number of logs per file per second: ";
	int nLogNumPerSecond = 0;
	scanf("%d", &nLogNumPerSecond);

	int nLogNumPer_50_ms = nLogNumPerSecond / 20; // 每50ms打印的log数
	int nLogNumPer_200_ms = nLogNumPerSecond / 5; // 每200ms打印的log数
	int nLogNumPer_500_ms = nLogNumPerSecond / 2; // 每500ms打印的log数

	if (nLogNumPer_50_ms > 0)
	{
		DiskIOTest(nLogFile, nLogNumPer_50_ms, 50);
	}
	else if (nLogNumPer_200_ms > 0)
	{
		DiskIOTest(nLogFile, nLogNumPer_200_ms, 200);
	}
	else if (nLogNumPer_500_ms > 0)
	{
		DiskIOTest(nLogFile, nLogNumPer_500_ms, 500);
	}
	else
	{
		DiskIOTest(nLogFile, nLogNumPerSecond, 1000);
	}

	std::system("pause");
}
