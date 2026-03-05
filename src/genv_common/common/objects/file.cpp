/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * file.cpp - Created on 01-06-2025
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

#include "file.hpp"
#include "common/services/services.hpp"
#include "common/return_codes.hpp"

namespace Files
{
    FileObject::~FileObject()
    {
        delete[] fileName;
        delete[] filePath;
        closeFile();
    }

    // Open file, optionally lock for writing else will be opened as read-only
    int FileObject::openFile(const char *filePath, bool lock)
    {
        this->filePath = filePath;
        this->fileName = getFileNamePos(this);
        return getServiceManager()->getStorage()->openFile(filePath, lock, this);
    }
    int FileObject::saveFile()
    {
        return getServiceManager()->getStorage()->writeFile(this);
    }
    int FileObject::renameFile(const char *fileName)
    {
        if (fileName != nullptr)
        {
            this->fileName = fileName;
            return getServiceManager()->getStorage()->renameFile(fileName, this);
        }
        return GV_ERROR(GV_SERVICE_FILESYSTEM, GV_CATEGORY_GENERIC, GV_ERR_INVALID_PARAM);
    }
    int FileObject::closeFile()
    {
        int r    = getServiceManager()->getStorage()->closeFile(this);
        fileName = nullptr;
        filePath = nullptr;
        return r;
    }
    int FileObject::deleteFile()
    {
        int r    = getServiceManager()->getStorage()->deleteFile(this);
        fileName = nullptr;
        filePath = nullptr;
        return r;
    }

    const char *getFileNamePos(FileObject *fObj)
    {
        if (fObj == nullptr)
            return nullptr;

        const char *path = fObj->getFilePath();
        unsigned int len = strlen(path);
        if (path == nullptr || len < 1)
            return nullptr;

        char *tempstr = new char[len];
        if (tempstr != nullptr)
        {
            strcpy(tempstr, path);
            char *token = strtok(tempstr, "\\");
            char *test  = token;
            if (token != nullptr)
            {
                while (test != nullptr)
                {
                    test = strtok(nullptr, "\\");
                    if (test == nullptr)
                    {
                        return strstr(path, token);
                    }
                    else
                    {
                        token = test;
                    }
                }
            }
            delete[] tempstr;
        }
        return nullptr;
    }

    util::Hash getFileNameHash(FileObject *fObj)
    {
        if (fObj != nullptr)
        {
            const char *name = fObj->getFileName();
            if (name == nullptr)
            {
                name = getFileNamePos(fObj);
                if (name == nullptr)
                    return 0;
            }
            return util::Hash(*name);
        }
        return 0;
    }
} // namespace Files