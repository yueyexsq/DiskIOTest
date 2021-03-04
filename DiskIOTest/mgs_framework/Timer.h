/* 
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef MANGOS_TIMER_H
#define MANGOS_TIMER_H

#include "Platform/CompilerDefs.h"

#if PLATFORM == PLATFORM_WINDOWS
#   include <mmsystem.h>
#   include <time.h>
#else
# if defined(__APPLE_CC__)
#   include <time.h>
# endif
#   include <sys/time.h>
#   include <sys/timeb.h>
#endif

#if PLATFORM == PLATFORM_WINDOWS
inline uint32 getMSTime() { return GetTickCount(); }
#else
inline uint32 getMSTime()
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday( &tv, &tz );
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
#ifndef __min
#define __min(a, b) min(a, b)
#endif
#ifndef __max
#define __max(a, b) max(a, b)
#endif
#endif

inline uint32 getMSTimeDiff(uint32 oldMSTime, uint32 newMSTime)
{
    // getMSTime() have limited data range and this is case when it overflow in this tick
    if (oldMSTime > newMSTime)                    
        return (0xFFFFFFFF - oldMSTime) + newMSTime;
    else
        return newMSTime - oldMSTime;
}

class IntervalTimer
{
    public:
        IntervalTimer() : _interval(0), _current(0) {}

        void Update(time_t diff) { _current += diff; if(_current<0) _current=0;}
        bool Passed() { return _current >= _interval; }
        void Reset() { if(_current >= _interval) _current -= _interval;  }

        void SetCurrent(time_t current) { _current = current; }
        void SetInterval(time_t interval) { _interval = interval; }
        time_t GetInterval() const { return _interval; }
        time_t GetCurrent() const { return _current; }

    private:
        time_t _interval;
        time_t _current;
};

//////////////////////////////////////////////////////////////////////
// 超时类，以“毫秒”为单位。需驱动Update，可暂停
//////////////////////////////////////////////////////////////////////
struct TimeTracker
{
    TimeTracker(time_t expiry) : i_expiryTime(expiry), i_interval(expiry) {}
    void Update(time_t diff) { i_expiryTime -= diff; }
    bool Passed(void) const { return (i_expiryTime <= 0); }
    void Reset(time_t interval) { i_expiryTime = interval; }
    void Reset() { i_expiryTime = i_interval; }
    time_t GetExpiry(void) const { return i_expiryTime; }
    time_t i_expiryTime;
    time_t i_interval;
};

struct TimeTrackerSmall
{
    TimeTrackerSmall(int32 expiry=0) : i_expiryTime(expiry), i_initTime(expiry) {}
    void Update(int32 diff) { i_expiryTime -= diff; }
	bool IsActive() { return i_initTime != 0 ? true : false; }
    bool Passed(void) const { return (i_expiryTime <= 0); }
	bool PassedByLoop(void) 
	{
		if (i_expiryTime <= 0)
		{
			i_expiryTime += i_initTime;
			return true;
		}
		return false;
	}
	void Reset(int32 interval) { i_initTime = i_expiryTime = interval; }
    void Reset() { i_expiryTime = i_initTime; }
	void ResetInterval(int32 interval)	{ i_expiryTime = interval; i_initTime=interval;}
    int32 GetExpiry(void) const { return i_expiryTime; }
    int32 i_expiryTime;
    int32 i_initTime;
};

//////////////////////////////////////////////////////////////////////
// 超时类，以“秒”为单位。无需驱动，不可暂停
//////////////////////////////////////////////////////////////////////
class CTimer  
{
public:
	CTimer(int nIntervalSecs = 0) { m_nInterval = nIntervalSecs; m_tUpdateTime = 0; }		// =0 : 用于数组初始化
	// virtual ~CTimer() {}
	CTimer& operator = (const CTimer& rhs)
	{
		m_nInterval		= rhs.m_nInterval;
		m_tUpdateTime	= rhs.m_tUpdateTime;
		return *this;
	}

public:
	// 设置时间标签
	void	Update		(void)					{ m_tUpdateTime = ::time(NULL); }

	// 用于定时触发的连续时钟。每次触发时为true。
	bool	ToNextTime	(void)					{ if(this->IsTimeOut()) return this->Update(),true; else return false; }

	// 初始化时间片。(但不启动
	void	SetInterval	(int nSecs)				{ m_nInterval = nSecs; }

	// 开始启动时钟(同SetInterval)
	void	Startup		(int nSecs)				{ m_nInterval = nSecs; this->Update(); }
	void	Startup		()						{ this->Update(); }

	// 时钟是否触发(只触发一次)，与Startup配合使用，可实现只触发一次的时钟。
	bool	TimeOver	(void)					{ if(this->IsActive() && this->IsTimeOut()) return this->Clear(),true; return false; }
	void	Clear		(void)					{ m_tUpdateTime = m_nInterval = 0; }
	bool	IsActive	(void)					{ return m_tUpdateTime != 0; }
	void	IncInterval	(int nSecs, int nLimit)	{ m_nInterval = __min(m_nInterval+nSecs, nLimit); }
	void	DecInterval	(int nSecs)				{ m_nInterval = __max(m_nInterval-nSecs, 0); }

	// 检查是否超时
	bool	IsTimeOut	(void)					{ return ::time(NULL) >= m_tUpdateTime+m_nInterval; }

	// 检查是否超时了指定时间片。用于同一时间标签检查多个不同超时事件。(可实现一个对象控制多个时钟)
	bool	IsTimeOut	(int nSecs)				{ return ::time(NULL) >= m_tUpdateTime+nSecs; }

	// 用于变长的超时事件。
	bool	ToNextTime	(int nSecs)				{ if(this->IsTimeOut(nSecs)) return this->Update(),true; else return false; }

	// 时钟会积累，但积累值不超过间隔值。其它同ToNextTime(...)
	bool	ToNextTick	(int nSecs);
	//	void	AppendInterval	(int nSecs)			{ if(ToNextTime()) m_nInterval=nSecs; else m_nInterval+=nSecs; }	// ToNextTime(): 保证至少有nSecs秒

	bool  MakeTimeOut(void)
	{
		m_tUpdateTime = (clock()/1000);
		if(m_tUpdateTime > m_nInterval)
		{
			m_tUpdateTime -= m_nInterval;
			return true;
		}
		else
		{
			m_tUpdateTime = 1;		
		}
		return false;
	}	

public: // get
	int		GetRemain()					const	{ return m_tUpdateTime ? __min(__max(m_nInterval - ((int)::time(NULL)-(int)m_tUpdateTime), 0), m_nInterval) : 0; }
	int		GetInterval()						{ return m_nInterval;}
	void	StartDayCheck()						{ Startup((int)GetRemainSecsToZero()); }
	bool	IsDayPassed()						{ if(IsActive() && IsTimeOut()) return Startup((int)GetRemainSecsToZero()), true; return false; }
	time_t  GetRemainSecsToZero();

protected:
	int		m_nInterval;
	time_t	m_tUpdateTime;
};


//////////////////////////////////////////////////////////////////////
// 超时类，以“毫秒”为单位。无需驱动，不可暂停
//////////////////////////////////////////////////////////////////////
class CTimerMS
{
public:
	CTimerMS(int nIntervalSecs = 0) { m_nInterval = nIntervalSecs; m_tUpdateTime = 0; }		// =0 : 用于数组初始化
	// virtual ~CTimerMS() {}
	CTimerMS& operator = (const CTimerMS& rhs)
	{
		m_nInterval		= rhs.m_nInterval;
		m_tUpdateTime	= rhs.m_tUpdateTime;
		return *this;
	}

public:
	void	Update		(void)					{ m_tUpdateTime = clock(); }
	bool	IsTimeOut	(void)					{ return clock() >= m_tUpdateTime+m_nInterval; }
	bool	ToNextTime	(void)					{ if(IsTimeOut()) return Update(),true; else return false; }
	void	SetInterval	(int nMilliSecs)		{ m_nInterval = nMilliSecs; }

	void	Startup		(int nMilliSecs)		{ m_nInterval = nMilliSecs; Update(); }
	void	Startup		()						{ this->Update(); }
	bool	TimeOver	(void)					{ if(IsActive() && IsTimeOut()) return Clear(),true; return false; }
	void	Clear		(void)					{ m_nInterval = m_tUpdateTime = 0; }
	bool	IsActive	(void)					{ return m_tUpdateTime != 0; }
	void	IncInterval	(int nMilliSecs, int nLimit)		{ m_nInterval = __min(m_nInterval+nMilliSecs, nLimit); }
	void	DecInterval	(int nMilliSecs)		{ m_nInterval = __max(m_nInterval-nMilliSecs, 0); }

	bool	IsTimeOut	(int nMilliSecs)		{ return clock() >= m_tUpdateTime+nMilliSecs; }
	bool	ToNextTime	(int nMilliSecs)		{ if(IsTimeOut(nMilliSecs)) return Update(),true; else return false; }
	bool	ToNextTick	(int nMilliSecs);
	//	void	AppendInterval	(int nMilliSecs)	{ if(ToNextTime()) m_nInterval=nMilliSecs; else m_nInterval+=nMilliSecs; }	// ToNextTime(): 保证至少有nSecs秒

public: // get
	int		GetInterval	(void)					{ return m_nInterval; }
	DWORD	GetUpdateTime(void)					{ return m_tUpdateTime; }
	int		GetPassedMS()						{ if (IsActive()) return clock()-m_tUpdateTime; return 0;}

protected:
	int		m_nInterval;
	clock_t	m_tUpdateTime;
};

#endif
