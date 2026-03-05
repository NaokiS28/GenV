/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * psx_mc.hpp - Created on 23-08-2025
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

#pragma once

#include <assert.h>

#include "psx_sio0.hpp"

#include "psx/common/psx_strings.hpp"

#include "common/logger/log.hpp"
#include "common/objects/file.hpp"
#include "common/services/storage/iface_storage.hpp"
#include "common/services/io/iface_driver.hpp"

// TODO: Implement memory card as a virtual file system complete with directory listing.

namespace PSX::IO
{
    class PSX_MemoryCard : public Files::IStorageDriver, public ::IO::IDriver
    {
    private:
        SIO0_Bus *m_bus;
        static uint8_t driverCount;
        const SIO0_Port _portNumber;

    public:
        inline PSX_MemoryCard(SIO0_Bus *bus, SIO0_Port port) : m_bus(bus), _portNumber(port)
        {
            assert(driverCount < 2);
            name = PSX_PS_MEMCARD_STR;
        };

        int init() override
        {
            LOG("psxmcd", "Init PlayStation Memory Card driver on port %d", (_portNumber == SIO0_Port::PORT1 ? 1 : 2));
            return 0;
        }

        bool update() override
        {
            return false;
        }

        bool reset() override { return false; }
        void shutdown() override { return; }

        int openFile(const char *filePath, bool lock, Files::FileObject *fObj);
        int closeFile(Files::FileObject *fObj);
        int writeFile(Files::FileObject *fObj);
        int renameFile(const char *fileName, Files::FileObject *fObj);
        int newFile(const char *filePath, const char *filename, Files::FileObject *fObj);
        int deleteFile(Files::FileObject *fObj);
        int readFile(size_t offset, size_t length);

        int readSector(uint16_t address, uint8_t *data, size_t length);
        int writeSector(uint16_t address, uint8_t *data, size_t length);
        uint16_t getID();

        // Gets a list of drives present in the system and returns the total count.
        int getDriveList(Files::IStorageDevice *list, uint8_t &count);
    };
} // namespace PSX::IO