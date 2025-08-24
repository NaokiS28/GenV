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

#include "common/objects/file.hpp"
#include "common/services/storage/iface_storage.hpp"

namespace System::PSX
{
    class PSX_CDROM : public Files::IStorage
    {
        virtual int init() { return 0; };
        virtual void update() {};
        virtual bool reset() { return false; };
        virtual void shutdown() {};

        virtual int openFile(const char *filePath, bool lock, Files::FileObject *fObj)
        {
            return 0;
        }
        virtual int closeFile(Files::FileObject *fObj)
        {
            return 0;
        }
        virtual int writeFile(Files::FileObject *fObj)
        {
            return 0;
        }
        virtual int renameFile(const char *fileName, Files::FileObject *fObj)
        {
            return 0;
        }
        virtual int newFile(const char *filePath, const char *filename, Files::FileObject *fObj)
        {
            return 0;
        }
        virtual int deleteFile(Files::FileObject *fObj)
        {
            return 0;
        }
        virtual int readFile(size_t offset, size_t length)
        {
            return 0;
        }

        // Gets a list of drives present in the system and returns the total count.
        virtual int getDriveList(Files::IStorageDevice *list, uint8_t &count)
        {
            return 0;
        }
    };
} // namespace System::PSX