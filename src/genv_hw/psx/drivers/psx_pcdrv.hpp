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

namespace System::PSX::Storage
{
    class PSX_PCDrive : public Files::IStorage
    {
        int init();
        void update();
        bool reset();
        void shutdown();

        int openFile(const char *filePath, bool lock, Files::FileObject *fObj);
        int closeFile(Files::FileObject *fObj);
        int writeFile(Files::FileObject *fObj);
        int renameFile(const char *fileName, Files::FileObject *fObj);
        int newFile(const char *filePath, const char *filename, Files::FileObject *fObj);
        int deleteFile(Files::FileObject *fObj);
        int readFile(size_t offset, size_t length);

        // Gets a list of drives present in the system and returns the total count.
        int getDriveList(Files::IStorageDevice *list, uint8_t &count);
    };
} // namespace System::PSX::Storage