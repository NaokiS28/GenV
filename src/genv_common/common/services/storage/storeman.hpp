/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * storeman.hpp - Created on 22-08-2025
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

#include "common/services/storage/iface_storage.hpp"
#include "common/services/storage/storage.hpp"
#include "common/services/adminkey.hpp"

namespace Files
{
    class StorageManager : public IStorage
    {
    private:
        IStorageDevice *deviceList = nullptr;
        int deviceListLength       = 0;

    public:
        StorageManager(AdminClass_Key key);
        ~StorageManager();

        int attachDevice(Files::IStorageDevice *dev);
        int detachDevice(Files::IStorageDevice *dev);

        int registerDriver(Files::IStorageDriver *dev);
        int unregisterDriver(Files::IStorageDriver *dev);

        int init() override { return 0; };
        bool reset() override { return true; }
        void update() override {};
        void shutdown() override {};

        int openFile(const char *filePath, bool lock, FileObject *fObj) override
        {
            return 0;
        }
        int closeFile(FileObject *fObj) override
        {
            return 0;
        }
        int writeFile(FileObject *fObj) override
        {
            return 0;
        }
        int renameFile(const char *fileName, FileObject *fObj) override
        {
            return 0;
        }
        int newFile(const char *filePath, const char *filename, FileObject *fObj) override
        {
            return 0;
        }
        int deleteFile(FileObject *fObj) override
        {
            return 0;
        }
        int readFile(size_t offset, size_t length) override
        {
            return 0;
        }

        int getDriveList(IStorageDevice *list, uint8_t &count) override
        {
            return 0;
        };
    };
} // namespace Files