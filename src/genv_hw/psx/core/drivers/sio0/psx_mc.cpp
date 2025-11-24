/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * psx_mc.cpp - Created on 08-11-2025
 *
 * GenV is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * GenV is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * GenV. If not, see <https://www.gnu.org/licenses/>.
 */

#include "psx_mc.hpp"
#include "psx/core/file/memcard.hpp"
#include "psx_sio0.hpp"

#define START(addr, port)                                   \
    {                                                       \
        int r = 0;                                          \
        if (r = psx_sio0.start(addr, port), r != SIO0_OKAY) \
        {                                                   \
            switch (r)                                      \
            {                                               \
            case SIO0_IN_USE: return 3;                     \
            case SIO0_NO_RESPONSE: return 1;                \
            }                                               \
        }                                                   \
    }

#define END() psx_sio0.stop()

namespace System::PSX::IO
{
    uint8_t PSX_MemoryCard::driverCount = 0;

    class Memcard_SectorRead
    {
    private:
        //  flag, id, null (address), ack, address ack, data, sum, end byte
        uint8_t rawData[1 + 2 + 2 + 2 + 2 + 128 + 1 + 1] = {
            CMD_WRITE_SECTOR, 0, 0};

    public:
        uint8_t *asBytes()
        {
            return rawData;
        }

        inline uint8_t flag() { return rawData[0]; }
        inline uint16_t id() { return (rawData[1] << 8) | rawData[2]; }
        inline uint16_t cmdAck() { return (rawData[5] << 8) | rawData[6]; }
        inline uint16_t addrAck() { return (rawData[7] << 8) | rawData[8]; }
        inline uint8_t *data() { return &rawData[9]; }
        inline uint8_t checksum() { return rawData[sizeof(rawData) - 2]; }
        inline uint8_t endByte() { return rawData[sizeof(rawData) - 1]; }

        bool validate() { return (PSX_MemoryCard_xor(&rawData[7], 130) == checksum()); }
    };

    class Memcard_SectorWrite
    {
    private:
        //  mcAddr, cmd, null (id), address, data, sum
        uint8_t rawData[1 + 1 + 2 + 2 + 128 + 1] = {
            CMD_WRITE_SECTOR, 0, 0};

    public:
        Memcard_SectorWrite(uint16_t _address, uint8_t *_data, size_t _length)
        {
            uint16_t address = (_address & 0x3FFF);
            rawData[3]       = (address & 0xff00) >> 8;
            rawData[4]       = (address & 0xff);
            memcpy(&rawData[5], _data, _length);
            rawData[(sizeof(rawData) - 1)] = PSX_MemoryCard_xor(&rawData[3], (sizeof(address) + _length));
        }

        const uint8_t *asBytes() const
        {
            return rawData;
        }
    };

    int PSX_MemoryCard::openFile(const char *filePath, bool lock, Files::FileObject *fObj)
    {
        return 0;
    }
    int PSX_MemoryCard::closeFile(Files::FileObject *fObj)
    {
        return 0;
    }
    int PSX_MemoryCard::writeFile(Files::FileObject *fObj)
    {
        return 0;
    }
    int PSX_MemoryCard::renameFile(const char *fileName, Files::FileObject *fObj)
    {
        return 0;
    }
    int PSX_MemoryCard::newFile(const char *filePath, const char *filename, Files::FileObject *fObj)
    {
        return 0;
    }
    int PSX_MemoryCard::deleteFile(Files::FileObject *fObj)
    {
        return 0;
    }
    int PSX_MemoryCard::readFile(size_t offset, size_t length)
    {
        return 0;
    }

    int PSX_MemoryCard::readSector(uint16_t address, uint8_t *data, size_t length)
    {
        if (!data || !length || address > 0x3FFF)
            return 1;

        START(ADDR_MEMORY_CARD, _portNumber);

        uint8_t aHi = (address & 0xff00) >> 8;
        uint8_t aLo = (address & 0xff);
        const uint8_t request[]{CMD_READ_SECTOR, 0, 0, aHi, aLo};

        Memcard_SectorRead response;

        size_t respLength = psx_sio0.exchangeBytes(
            request,
            response.asBytes(),
            sizeof(request),
            sizeof(response));
        END();

        if (response.validate())
            return 5;

        if (respLength < 4)
            return 1;
        return response.endByte();
    }

    int PSX_MemoryCard::writeSector(uint16_t address, uint8_t *data, size_t length)
    {
        if (!data || !length || address > 0x3FFF)
            return 1;

        Memcard_SectorWrite request(address, data, length);

        START(ADDR_MEMORY_CARD, _portNumber);
        uint8_t response[sizeof(request) + 3]; // Ack + endbyte
        size_t respLength = psx_sio0.exchangeBytes(
            request.asBytes(),
            response,
            sizeof(request),
            sizeof(response));
        END();

        if (respLength < sizeof(request) + 3)
            return 1;

        int rxRsp      = sizeof(response) - 3;
        uint16_t ack   = (response[rxRsp] << 8) | response[rxRsp + 1];
        uint8_t result = response[rxRsp + 2];

        if (ack != 0x5C5D)
            return 5;
        return result;
    }

    uint16_t PSX_MemoryCard::getID()
    {
        uint8_t request[]{CMD_IDENTIFY_CARD, 0, 0, 0};
        uint8_t response[8];

        START(ADDR_MEMORY_CARD, _portNumber);
        size_t respLength = psx_sio0.exchangeBytes(
            request,
            response,
            sizeof(request),
            sizeof(response));
        END();

        if (respLength < 9)
            return 3;
        return 0;
    }

    // Gets a list of drives present in the system and returns the total count.
    int PSX_MemoryCard::getDriveList(Files::IStorageDevice *list, uint8_t &count)
    {
        return 0;
    }
} // namespace System::PSX::IO