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

#ifndef MANGOSSERVER_WORLDPACKET_H
#define MANGOSSERVER_WORLDPACKET_H

//#include "BaseCommon.h"
#include "ByteBuffer.h"

// typedef tq::CMsg WorldPacket;

#include <Types.h>
/*#include <MsgDefine.h>*/
// #include "MsgTypeDefine.h"
/*#include <BaseCode.h>*/
#include "ByteBuffer.h"

const int _NONE_MSG = 0;

// msg head
struct  MSG_HEAD
{
    USHORT usSize;
    USHORT usType;
};

class WorldPacket : public ByteBuffer
{
public:
    WorldPacket() : ByteBuffer(2048)//,m_extBuf(NULL)
    {
    }

    explicit WorldPacket(uint16 opcode, size_t res=2048) : ByteBuffer(res)//,m_extBuf(NULL)
    {
        append<uint16>((uint16)res);
        append<uint16>(opcode);
        //         IF_OK(res>4)
        //         {
        //             uint8 v=0;
        //             append<uint8>(&v,res-4);
        //         }
    }

    WorldPacket(const WorldPacket &msg) : ByteBuffer(msg)//,m_extBuf(NULL)
    {
    }

    virtual ~WorldPacket()
    {}

	virtual void Reset()
	{
		clear();
	}

    const char* GetBuf(void)
    {
//         if (m_extBuf)
//         {
//             return m_extBuf;
//         }
//         else
        {
            UpdateSize();
            return (char*)contents();
        }
    }

    MSG_HEAD* GetHead(void) const
    {
        return (MSG_HEAD*)contents();
    }

    USHORT GetSize(void) const
    {
        return (USHORT)size();
    }

    void UpdateSize()
    {
        GetHead()->usSize = size();
    }

    USHORT GetType(void) const
    {
        return GetHead()->usType;
    }

    USHORT GetHeadSize(void)
    {
        return sizeof(MSG_HEAD);
    }

    bool IsValid(void)
    {
        if(this->GetHeadSize() >= this->GetSize())
            return false;

        if(_NONE_MSG == this->GetType())
            return false;

        return true;
    }

protected:
    void Initialize(uint16 opcode, size_t newres=0)
    {
//         m_extBuf=NULL;
        clear();
        _storage.reserve(newres);
        append<uint16>((uint16)newres);
        append<uint16>(opcode);
        //if(newres>4)
        //{
        //    uint8 v=0;
        //    append<uint8>(&v,newres-4);
        //}
		if (newres > size())
		{
			resize(newres);
		}
    }
// private:
//     char* m_extBuf;
public:
    virtual bool Create(const char* pszBuf, USHORT usBufLen)
    {
        if(!pszBuf)
            return false;

        MSG_HEAD* pHead = (MSG_HEAD*)pszBuf;
        if((USHORT)usBufLen != pHead->usSize)
            return false;

        if(_NONE_MSG == pHead->usType)
            return false;

//         m_extBuf=NULL;
		clear();
        append(pszBuf,usBufLen);
        return true;
    }

//     virtual bool CreateNoCopy(char* pszBuf, USHORT usBufLen)
//     {
//         if(!pszBuf)
//             return false;
// 
//         MSG_HEAD* pHead = (MSG_HEAD*)pszBuf;
//         if((USHORT)usBufLen != pHead->usSize)
//             return false;
// 
//         if(_MSG_NONE == pHead->usType)
//             return false;
// 
//         clear();
//         m_extBuf = pszBuf;
//         return true;
//     }

	virtual void Process(void* pInfo)
	{
		//         UNUSED_ARG(pInfo);
		/*        tq::LogSave("Module", "tq::CMsg::Process(void* pInfo) error");*/
	}

    virtual void Process(void* pInfo, unsigned idComponent)
    {
//         UNUSED_ARG(pInfo);
/*        tq::LogSave("Module", "tq::CMsg::Process(void* pInfo) error");*/
    }

    virtual void Process(OBJID idActor)
    {
//         UNUSED_ARG(idActor);
/*        tq::LogSave("Module", "tq::CMsg::Process(OBJID idActor) error");*/
    }

    virtual void Process(OBJID idActor, void* pInfo)
    {
//         UNUSED_ARG(idActor);
//         UNUSED_ARG(pInfo);
/*        tq::LogSave("Module", "tq::CMsg::Process(OBJID idActor, void* pInfo) error");*/
    }
};

#endif
