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

#ifndef MANGOSSERVER_PATH_H
#define MANGOSSERVER_PATH_H

// #include "Define.h"
#include <vector>
#ifndef UINT_MAX
#define UINT_MAX 0xffffffff
#endif
class Path
{
    public:
        struct PathNode
        {
            float x,y,z;
			union
			{
				float time;
				float dis;
			};
        };

		typedef std::vector<PathNode> VEC_PATHNODE;

   //     void SetLength(const unsigned int sz)
   //     {
   //         //i_nodes.resize( sz );
			//i_nodes.reserve(sz);
   //     }

		unsigned int Size() const { return (unsigned int)i_nodes.size(); }
		unsigned int size() const { return (unsigned int)i_nodes.size(); }
        bool Empty() const { return i_nodes.empty(); }
        bool empty() const { return i_nodes.empty(); }
        void Resize(unsigned int sz) { i_nodes.resize(sz); }
        void resize(unsigned int sz) { i_nodes.resize(sz); }
		void Reserve(unsigned int sz) { i_nodes.reserve(sz); }
		void reserve(unsigned int sz) { i_nodes.reserve(sz); }
        void Clear(void) { i_nodes.clear(); }
        void clear(void) { i_nodes.clear(); }
		PathNode& GetNode(uint32 index) { return i_nodes[index]; }

        float GetPassedLength(uint32 curnode, float x, float y, float z)
        {
            float len = 0, xd, yd, zd;
            for(unsigned int idx=1; idx < curnode; ++idx)
            {
                xd = i_nodes[ idx ].x - i_nodes[ idx-1 ].x;
                yd = i_nodes[ idx ].y - i_nodes[ idx-1 ].y;
                zd = i_nodes[ idx ].z - i_nodes[ idx-1 ].z;
                len += sqrtf( xd*xd + yd*yd + zd*zd );
            }

            if(curnode > 0)
            {
                xd = x - i_nodes[curnode-1].x;
                yd = y - i_nodes[curnode-1].y;
                zd = z - i_nodes[curnode-1].z;
                len += sqrtf( xd*xd + yd*yd + zd*zd );
            }

            return len;
        }

		void AddByTime(float x, float y, float z, float time)
		{
			PathNode node;
			node.x = x;
			node.y = y;
			node.z = z;
			node.time = time;
			i_nodes.push_back(node);
		}

		void AddByDis(float x, float y, float z, float dis)
		{
			PathNode node;
			node.x = x;
			node.y = y;
			node.z = z;
			node.dis = dis;
			i_nodes.push_back(node);
		}

		void Join(Path& path)
		{
			if (path.i_nodes.empty())
				return;
			i_nodes.insert(i_nodes.end(), path.i_nodes.begin(), path.i_nodes.end());
		}

		void Join(Path& path, uint32 startIndex)
		{
			if (path.i_nodes.empty())
				return;
			if (startIndex > (uint32)path.i_nodes.size())
				startIndex = (uint32)path.i_nodes.size();
			i_nodes.insert(i_nodes.begin()+startIndex, path.i_nodes.begin(), path.i_nodes.end());
		}

		void Assign(Path& path)	{Clear(); i_nodes = path.GetNodeSet();}
		const VEC_PATHNODE& GetNodeSet()	{return i_nodes;}

        PathNode& operator[](const unsigned int idx) { return i_nodes[idx]; }
        const PathNode& operator()(const unsigned int idx) const { return i_nodes[idx]; }
        PathNode& at(const unsigned int idx) { return i_nodes[idx]; }
    protected:
        VEC_PATHNODE i_nodes;
};

struct PathMoveInfo
{
	unsigned int curNode = 0;
	float curX = 0.0;
	float curY = 0.0;
	float curZ = 0.0;
	float totalTime = 0.0;
	
	union
	{
		float time = 0.0;
		float dis;
	} cur;
};

struct TempPathMoveInfo
{
	PathMoveInfo info;

	void Enable() { m_bEnable = true; }
	void DisEnable(){ m_bEnable = false; }
	bool IsEnable(){ return m_bEnable; }
	void Clear()
	{
		info.curNode = 0;
		info.curX = 0.0;
		info.curY = 0.0;
		info.curZ = 0.0;
		info.totalTime = 0.0;
		info.cur.time = 0.0;
	}
	bool m_bEnable = false;
};

class PathMove : public Path
{
public:
	PathMove(bool b3D = false)
    {
        Clear();
		m_b3D = b3D;
    }
    bool UpdateNode(float diff)
    {
		if (!CheckStart())
			return false;

        if (m_info.curNode >= i_nodes.size())
        {
            return true;
        }

		TempPathMoveInfo& temp = GetTempInfo();
		if (!temp.IsEnable())
		{
			if (!TryUpdateNode(diff))
				return false;
		}

		m_info.curNode = temp.info.curNode;
		m_info.curX = temp.info.curX;
		m_info.curY = temp.info.curY;
		m_info.curZ = temp.info.curZ;
		m_info.totalTime = temp.info.totalTime;
		m_info.cur = temp.info.cur;

		temp.DisEnable();
        return true;
    }

	//调用GetFuturePos 后必须接着调用UpdateNode。因为GetFuturePos 会把运算结果保存到静态变量里，UpdateNode会直接使用。
    bool GetFuturePos(float time,float& newX,float& newY,float& newZ)
    {
		if (!TryUpdateNode(time))
			return false;

		TempPathMoveInfo& temp = GetTempInfo();
		if (!temp.IsEnable())
			return false;

		newX = temp.info.curX;
		newY = temp.info.curY;
		newZ = temp.info.curZ;
		return true;
    }

	bool TryUpdateNode(float diff)
	{
		if (Empty()) return false;
		TempPathMoveInfo& temp = GetTempInfo();
		temp.info.curNode = m_info.curNode;
		temp.info.curX = m_info.curX;
		temp.info.curY = m_info.curY;
		temp.info.curZ = m_info.curZ;
		temp.info.totalTime = m_info.totalTime;
		temp.info.cur = m_info.cur;

		temp.Enable();
		if (m_info.curNode == UINT_MAX)
		{
			temp.info.curNode = 0;
			temp.info.curX = i_nodes[0].x;
			temp.info.curY = i_nodes[0].y;
			temp.info.curZ = i_nodes[0].z;
		}

		if (m_info.curNode >= i_nodes.size())
		{
			temp.info.curX = m_info.curX;
			temp.info.curY = m_info.curY;
			temp.info.curZ = m_info.curZ;
			return true;
		}

		if (0.0 == m_acceleration)//匀速运动
		{
			temp.info.totalTime = m_info.totalTime + diff;
			temp.info.cur.time = m_info.cur.time;
			unsigned int nodeCount = Size();
			for (; temp.info.curNode < nodeCount; ++temp.info.curNode)
			{
				//到达终点
				if ((temp.info.curNode + 1) >= nodeCount)
				{
					temp.info.curX = i_nodes[temp.info.curNode].x;
					temp.info.curY = i_nodes[temp.info.curNode].y;
					temp.info.curZ = i_nodes[temp.info.curNode].z;
					continue;
				}

				const float overTime = temp.info.totalTime - temp.info.cur.time;
				const float nextTime = i_nodes[temp.info.curNode + 1].time;
				if (nextTime <= overTime)
				{
					//超过当前节点
					temp.info.cur.time += nextTime;
					continue;
				}
				else
				{
					//节点内移动
					//计算当前节点偏移
					float fx = i_nodes[temp.info.curNode].x;
					float fy = i_nodes[temp.info.curNode].y;
					float fz = i_nodes[temp.info.curNode].z;
					float dx = i_nodes[temp.info.curNode + 1].x;
					float dy = i_nodes[temp.info.curNode + 1].y;
					float dz = i_nodes[temp.info.curNode + 1].z;
					float percent_passed = nextTime == 0.0 ? 1.0f : overTime / nextTime;
					temp.info.curX = fx + ((dx - fx) * percent_passed);
					temp.info.curY = fy + ((dy - fy) * percent_passed);
					temp.info.curZ = fz + ((dz - fz) * percent_passed);
					break;
				}
			}
		}
		else//匀变速运动
		{
			unsigned int nodeCount = Size();
			temp.info.totalTime = m_info.totalTime + diff;
			temp.info.cur.dis = m_info.cur.dis;

			//减速移动时，速度变为负值则立刻到达，避免往回走
			if ((m_speed + m_acceleration*temp.info.totalTime) <= 0)
			{
				GetEndPos(temp.info.curX, temp.info.curY, temp.info.curZ);
				temp.info.curNode = nodeCount - 1;
				return true;
			}
			
			float totalDis = m_speed*temp.info.totalTime + 0.5f*m_acceleration*temp.info.totalTime*temp.info.totalTime;
			for (; temp.info.curNode < nodeCount; ++temp.info.curNode)
			{
				//到达终点
				if ((temp.info.curNode + 1) >= nodeCount)
				{
					temp.info.curX = i_nodes[temp.info.curNode].x;
					temp.info.curY = i_nodes[temp.info.curNode].y;
					temp.info.curZ = i_nodes[temp.info.curNode].z;
					continue;
				}

				const float overDis = totalDis - temp.info.cur.dis;
				const float nextDis = i_nodes[temp.info.curNode + 1].dis;
				if (nextDis <= overDis)
				{
					//超过当前节点
					temp.info.cur.dis += nextDis;
					continue;
				}
				else
				{
					//节点内移动
					//计算当前节点偏移
					float fx = i_nodes[temp.info.curNode].x;
					float fy = i_nodes[temp.info.curNode].y;
					float fz = i_nodes[temp.info.curNode].z;
					float dx = i_nodes[temp.info.curNode + 1].x;
					float dy = i_nodes[temp.info.curNode + 1].y;
					float dz = i_nodes[temp.info.curNode + 1].z;
					float percent_passed = nextDis == 0.0 ? 1.0f : overDis / nextDis;
					temp.info.curX = fx + ((dx - fx) * percent_passed);
					temp.info.curY = fy + ((dy - fy) * percent_passed);
					temp.info.curZ = fz + ((dz - fz) * percent_passed);
					break;
				}
			}
		}
		return true;
	}

	TempPathMoveInfo& GetTempInfo()
	{
		return m_TempPathMoveInfo;
	}

    //bool GetFuturePath(PathMove &other)
    //{
    //    if(Arrived())
    //        return false;
 
    //    other.clear();

    //    //first path node
    //    other.Add(m_info.curX, m_info.curY, m_info.curZ, 0);

    //    //second path node;
    //    uint32 time = i_nodes[m_info.curNode+1].time + m_info.cur.time - m_info.totalTime;
    //    other.Add(i_nodes[m_info.curNode+1].x, i_nodes[m_info.curNode+1].y, i_nodes[m_info.curNode+1].z, time);

    //    //other path node
    //    for(uint32 i = m_info.curNode+2; i < Size(); i++)
    //        other.Add(i_nodes[i].x, i_nodes[i].y, i_nodes[i].z, i_nodes[i].time);

    //    return true;
    //}

	bool IsBlink() const { return 0.0 == m_speed; }
    bool IsValid() const {return !Empty();}
    void GetCurPos(float& x,float& y,float& z) const {x=m_info.curX;y=m_info.curY;z=m_info.curZ;}
    bool Arrived() const {return (IsValid()&&(m_info.curNode+1)>=Size());}
    void Clear()
    {
        Path::Clear();
        m_info.curNode=UINT_MAX;
        m_info.totalTime = 0.f;
        m_info.cur.time = 0.f;
        m_info.curX=0.f;
        m_info.curY=0.f;
        m_info.curZ=0.f;
		m_acceleration = 0.f;
		m_speed = 0.f;
    }
    void clear(){return Clear();}

	void Assign(Path& path)	{Clear();Path::Assign(path);}
	bool GetEndPos(float& x,float& y,float& z)
	{
		if(Empty()) return false;
		size_t i = i_nodes.size() - 1;
		x=i_nodes[i].x;
		y=i_nodes[i].y;
		z=i_nodes[i].z;
		return true;
	}

	unsigned GetCurNode() { return m_info.curNode; }

	bool CheckStart()
	{
		if (Empty())
			return false;

		if (m_info.curNode == UINT_MAX)
		{
			m_info.curNode = 0;
			m_info.curX = i_nodes[0].x;
			m_info.curY = i_nodes[0].y;
			m_info.curZ = i_nodes[0].z;
		}

		return true;
	}

	void AddNode(float x, float y, float z)
	{
		if (m_acceleration)
		{
			float dis = 0.0;
			if (!Empty())
			{
				float xd, yd, zd;
				xd = x - (*i_nodes.rbegin()).x;
				yd = y - (*i_nodes.rbegin()).y;
				if (m_b3D)
				{
					zd = z - (*i_nodes.rbegin()).z;
					dis = sqrtf(xd*xd + yd*yd + zd*zd);
				}
				else
				{
					dis = sqrtf(xd*xd + yd*yd);
				}
			}
			AddByDis(x, y, z, dis);
		}
		else
		{
			float time = 0;
			if (!Empty() && 0.0 != m_speed)
			{
				float xd, yd, zd;
				xd = x - (*i_nodes.rbegin()).x;
				yd = y - (*i_nodes.rbegin()).y;
				if (m_b3D)
				{
					zd = z - (*i_nodes.rbegin()).z;
					time = sqrtf(xd*xd + yd*yd + zd*zd) / m_speed;
				}
				else
				{
					time = sqrtf(xd*xd + yd*yd) / m_speed;
				}
			}
			AddByTime(x, y, z, time);
		}
	}

	//speed: 速度（距离单位/毫秒） acceleration： 加速度（距离单位/平方毫秒）
	void SetAcceleration(float speed, float acceleration)
	{
		m_speed = abs(speed);
		m_acceleration = acceleration;
	}

	float GetTotalLength() const { return GetTotalLength(0, Size() - 1); }
	float GetTotalLength(uint32 start, uint32 end) const
	{
		if (start >= end)
			return 0;

		if (end >= Size())
			end = Size() - 1;
		float len = 0, xd, yd, zd;
		for (unsigned int idx = start; idx < end; ++idx)
		{
			if (m_acceleration)
			{
				len += i_nodes[idx + 1].dis;
			}
			else
			{
				xd = i_nodes[idx + 1].x - i_nodes[idx].x;
				yd = i_nodes[idx + 1].y - i_nodes[idx].y;
				if (m_b3D)
				{
					zd = i_nodes[idx].z - i_nodes[idx - 1].z;
					len += sqrtf(xd*xd + yd*yd + zd*zd);
				}
				else
				{
					len += sqrtf(xd*xd + yd*yd);
				}
			}
		}
		return len;
	}

	//不可用于变速移动
	float GetTotalTime() const { return GetTotalTime(0, Size()); }
	float GetTotalTime(uint32 start, uint32 end) const
	{
		float time = 0;
		for (unsigned int idx = start + 1; idx < end; ++idx)
		{
			time += i_nodes[idx].time;
		}
		return time;
	}

	bool GetTotalLengthAndTime(float& length, float& time)
	{
		length = 0.0;
		time = 0.0;
		if (m_acceleration)
		{
			for (unsigned int idx = 1; idx < Size(); ++idx)
			{
				length += i_nodes[idx].dis;
			}

			//根据匀变速运动距离时间公式，已知初始速度、距离，求时间
			float d = m_speed*m_speed + 2.0f*m_acceleration*length;
			if (d >= 0)
			{
				time = (-m_speed + sqrt(d)) / m_acceleration;
			}
			else
			{
				//速度降为0依然未能到达目的点。则把速度为零定位到达时刻。
				if (m_acceleration < 0)
				{
					time = -m_speed / m_acceleration;
					length = -m_speed*m_speed / (2.0f*m_acceleration);
				}
			}
		}
		else
		{
			float xd, yd, zd;
			for (unsigned int idx = 1; idx < Size(); ++idx)
			{
				xd = i_nodes[idx].x - i_nodes[idx - 1].x;
				yd = i_nodes[idx].y - i_nodes[idx - 1].y;
				if (m_b3D)
				{
					zd = i_nodes[idx].z - i_nodes[idx - 1].z;
					length += sqrtf(xd*xd + yd*yd + zd*zd);
				}
				else
				{
					length += sqrtf(xd*xd + yd*yd);
				}
				time += i_nodes[idx].time;
			}
		}
		return true;
	}
protected:
	bool m_b3D = false;//是否3D 移动（默认2D移动）

public:
	PathMoveInfo m_info;
	float m_acceleration = 0.0;	//加速度(距离单位/平方毫秒)
	float m_speed = 0.0;		//速度（距离单位/毫秒）。0表示立刻到达

public:
	TempPathMoveInfo m_TempPathMoveInfo;//TODO 待优化。此对象一个线程只需要一个即可，作为成员变量的话浪费空间
};

#endif
