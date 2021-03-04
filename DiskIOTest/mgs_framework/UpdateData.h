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

#ifndef __UPDATEDATA_H
#define __UPDATEDATA_H
#include "BaseCommon.h"
#include "WorldPacket.h"


// enum OBJECT_UPDATE_TYPE
// {
//     UPDATETYPE_NONE					= 0,
//     UPDATETYPE_ADDPLAYER			= 1,
//     UPDATETYPE_DELPLAYER			= 2,
//     UPDATETYPE_PLAYERINFO			= 3,
//     UPDATETYPE_MOVEMENT				= 4,
// };

enum OBJECT_UPDATE_FLAGS
{
    UPDATEFLAG_NONE         = 0x00,
    UPDATEFLAG_SELF         = 0x01,
    UPDATEFLAG_TRANSPORT    = 0x02,
    UPDATEFLAG_FULLGUID     = 0x04,
    UPDATEFLAG_LOWGUID      = 0x08,
    UPDATEFLAG_HIGHGUID     = 0x10,
    UPDATEFLAG_LIVING       = 0x20,
    UPDATEFLAG_HASPOSITION  = 0x40
};

class UpdateData
{
    public:
        UpdateData();

        void AddOutOfRangeGUID(std::set<uint64>& guids);
        void AddOutOfRangeGUID(const uint64 &guid);
        void AddUpdateBlock(const ByteBuffer &block);
        bool BuildPacket(WorldPacket *packet, int32 nIndex = 0);
		bool BuildOutOfRangePacket(WorldPacket& packet);

        bool HasData();
		bool IsOutOfRangeData();
		bool IsBufferData();
        void Clear();

		void SetOpcode(uint16 nCode);
		uint16 GetOpcode();

		uint16 GetDataSize();

        std::set<uint64> const& GetOutOfRangeGUIDs() const { return m_outOfRangeGUIDs; }

    protected:
        uint32 m_blockCount;
        std::set<uint64> m_outOfRangeGUIDs;
        ByteBuffer m_data;
		std::vector<ByteBuffer> m_vecData;
		uint16 m_Opcode;
};
#endif
