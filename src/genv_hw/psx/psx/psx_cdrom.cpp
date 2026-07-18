/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * psx_cdrom.cpp - Created on 08-11-2025
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

#include "psx_cdrom.hpp"
#include "common/return_codes.hpp"

#include "common/logger/log.hpp"

namespace PS1::Storage
{

    int PS1_CDROM::init()
    {
        LOG("psxpad", "Init PlayStation CD-ROM driver");
        return GV_ERROR(GV_SERVICE_FILESYSTEM, GV_CATEGORY_GENERIC, GV_ERR_DEVICE_NOT_READY);
    };
    bool PS1_CDROM::update()
    {
        return 0;
    }
    bool PS1_CDROM::reset()
    {
        return false;
    };
    void PS1_CDROM::shutdown() {};

    int PS1_CDROM::openFile(const char *filePath, bool lock, Files::FileObject *fObj)
    {
        return 0;
    }
    int PS1_CDROM::closeFile(Files::FileObject *fObj)
    {
        return 0;
    }

    int PS1_CDROM::readFile(size_t offset, size_t length)
    {
        return 0;
    }

    // Gets a list of drives present in the system and returns the total count.
    /*int PS1_CDROM::getDriveList(Files::IStorageDevice *list, uint8_t &count)
    {
        count = 1;
        return 0;
    }*/
} // namespace PS1::Storage