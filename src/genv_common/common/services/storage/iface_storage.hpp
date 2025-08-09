/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * iface_storage.hpp - Created on 09-08-2025
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

namespace Files
{
    struct IStorageDevice
    {

    };

    class FileObject;

    class IStorage
    {
    public:
        IStorage() = default;
        virtual ~IStorage() {}

        virtual bool init() = 0;
        virtual bool reset() = 0;
        virtual void shutdown() = 0;

        virtual int openFile(const char *filePath, bool lock, FileObject *fObj) { return 0; }
        virtual int closeFile(FileObject *fObj) { return 0; }
        virtual int writeFile(FileObject *fObj) { return 0; }
        virtual int renameFile(const char *fileName, FileObject *fObj) { return 0; }
        virtual int newFile(const char *filePath, const char *filename, FileObject *fObj) { return 0; }
        virtual int deleteFile(FileObject *fObj) { return 0; }
        virtual int readFile(size_t offset, size_t length) { return 0; }

        // Gets a list of drives present in the system and returns the total count.
        virtual uint8_t getDriveList(IStorageDevice *list) = 0;
        virtual const char *getWorkingDirectory() = 0;

    private:
    };
}