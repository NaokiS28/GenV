/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * psx_pcdrv.cpp - Created on 08-11-2025
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

#include "psx_pcdrv.hpp"
#include "common/return_codes.hpp"

namespace System::PSX::Storage
{

    int PSX_PCDrive::init()
    {
        return GV_ERROR(GV_SERVICE_FILESYSTEM, GV_CATEGORY_GENERIC, GV_ERR_DEVICE_NOT_READY);
    }
    void PSX_PCDrive::update() {}
    bool PSX_PCDrive::reset()
    {
        return false;
    }
    void PSX_PCDrive::shutdown() {}

    int PSX_PCDrive::openFile(const char *filePath, bool lock, Files::FileObject *fObj)
    {
        return 0;
    }
    int PSX_PCDrive::closeFile(Files::FileObject *fObj)
    {
        return 0;
    }
    int PSX_PCDrive::writeFile(Files::FileObject *fObj)
    {
        return 0;
    }
    int PSX_PCDrive::renameFile(const char *fileName, Files::FileObject *fObj)
    {
        return 0;
    }
    int PSX_PCDrive::newFile(const char *filePath, const char *filename, Files::FileObject *fObj)
    {
        return 0;
    }
    int PSX_PCDrive::deleteFile(Files::FileObject *fObj)
    {
        return 0;
    }
    int PSX_PCDrive::readFile(size_t offset, size_t length)
    {
        return 0;
    }

    // Gets a list of drives present in the system and returns the total count.
    int PSX_PCDrive::getDriveList(Files::IStorageDevice *list, uint8_t &count)
    {
        return 0;
    }
} // namespace System::PSX::Storage