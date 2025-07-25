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

namespace Files
{
    FileObject::FileObject()
    {
        for (int i = 0; i < paramListSize; i++)
        {
            paramList[i].hash = paramEntryNull;
            paramList[i].param = 0;
        }
    }

    FileObject::~FileObject()
    {
        delete[] fileName;
        delete[] filePath;
        delete this->data;
        closeFile();
    }

    // Open file, optionally lock for writing else will be opened as read-only
    int FileObject::openFile(const char *filePath, bool lock)
    {
        this->filePath = filePath;
        this->fileName = getFileNamePos(this);
        return Services::getStorage()->openFile(filePath, lock, this);
    }
    int FileObject::saveFile()
    {
        return Services::getStorage()->writeFile(this);
    }
    int FileObject::renameFile(const char *fileName)
    {
        if (fileName != nullptr)
        {
            this->fileName = fileName;
            return Services::getStorage()->renameFile(fileName, this);
        }
        return Files::FO_ERROR_BADPARAM;
    }
    int FileObject::closeFile()
    {
        int r = Services::getStorage()->closeFile(this);
        fileName = nullptr;
        filePath = nullptr;
        delete this->data;
        return r;
    }
    int FileObject::deleteFile()
    {
        int r = Services::getStorage()->deleteFile(this);
        fileName = nullptr;
        filePath = nullptr;
        delete this->data;
        return r;
    }

    uint8_t FileObject::read()
    {
        if (!this->data->getRawData())
            throw std::runtime_error("No data loaded");

        uint8_t d = data->getRawData()[filePos++];

        if (filePos >= getSize())
        {
            if (allowWrap)
            {
                filePos = 0; // Wraparound or handle as error?
            }
            else
            {
                return UINT8_MAX;
            }
        }

        return d;
    }

    bool FileObject::read(uint8_t *buffer, size_t length, size_t bufferSize)
    {
        if (length < bufferSize && buffer)
        {
            for (; length > 0; length--)
            {
                buffer[length] = read();
            }
            return true;
        }
        return false;
    }

    uint8_t FileObject::peek()
    {
        if (!this->data->getRawData())
            return UINT8_MAX;

        if (filePos + 1 >= getSize())
            return UINT8_MAX;

        return this->data->getRawData()[filePos + 1];
    }

    bool FileObject::setParam(util::Hash object, size_t param)
    {
        uint8_t pos = getParamPos(object);
        if (pos == paramNotFound)
        {
            // Not in list
            for (int i = 0; i < paramListSize; i++)
            {
                if (paramList[i].hash == paramEntryNull)
                {
                    paramList[i].hash = object;
                    paramList[i].param = param;
                    return true;
                }
            }
            return false;
        }
        else
        {
            paramList[pos].hash = object;
            paramList[pos].param = param;
            return true;
        }
    }

    // Get metadata - Used for file handling
    bool FileObject::getParam(util::Hash object, size_t &param)
    {
        uint8_t pos = getParamPos(object);
        if (pos == paramNotFound)
        {
            param = paramEntryNull;
            return false;
        }
        param = paramList[pos].param;
        return true;
    }

    uint8_t FileObject::getParamPos(util::Hash object)
    {
        for (int i = 0; i < paramListSize; i++)
        {
            if (paramList[i].hash == object)
            {
                return i;
            }
        }
        return paramNotFound;
    }

    bool FileObject::deleteParam(util::Hash object)
    {
        uint8_t pos = getParamPos(object);
        if (pos != paramNotFound)
        {
            paramList[pos].hash = paramEntryNull;
            paramList[pos].param = 0;
            return true;
        }
        return false;
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
            char *test = token;
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
            return util::Hash(name);
        }
        return 0;
    }
}