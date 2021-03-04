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
#include <vector>

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
std::string GetFileName(struct tm tt, std::string strFileName, int fileIndex, int logIndex)
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
	chrono::steady_clock::time_point start, printLogTime,end;
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
