/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * file.hpp - Created on 12-05-2025
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
#include "data.hpp"
#include "common/util/hash.hpp"

namespace Files
{
    constexpr const uint8_t paramListSize = 10;
    constexpr const uint8_t paramNotFound = UINT8_MAX;
    constexpr const util::Hash paramEntryNull = UINT32_MAX;

    enum : uint8_t
    {
        FO_OKAY,                // Success
        FO_ERROR_OUTOFMEMORY,   // Couldn't allocate memory for file
        FO_ERROR_BADPERMISSION, // Permissions on file denied this action
        FO_ERROR_BADPATH,       // File path was invalid, badly formatted or has invalid characters
        FO_ERROR_FILENOTFOUND,  // File was not found
        FO_ERROR_BADOBJECT,     // DataObject is bad
        FO_ERROR_BADPARAM,      // A parameter provided did not meet the requirements
        FO_ERROR_INCOMPATIBLE,  // File type is incompatible with the target object
        FO_ERROR_BADNAME,       // Filename is invalid
        FO_ERROR_FILEINUSE,     // File is already open and locked for writing
        FO_ERROR_NOLOCK,        // File is not locked or lock could not be set
        FO_ERROR_INVALIDFUNC,   // Function has not been provided on this file provider.
        FO_ERROR_UNKNOWN        // Unknown or unexpected error occured
    };

    struct FileParameter
    {
        util::Hash hash = 0;
        size_t param = 0;
    };

    class FileObject
    {

    public:
        FileObject();
        FileObject(const char *filePath, bool lock = false)
        {
            openFile(filePath, lock);
        }
        virtual ~FileObject();

        // Open file, optionally lock for writing else will be opened as read-only
        virtual int openFile(const char *filePath, bool lock = false);
        virtual int saveFile();
        virtual int renameFile(const char *fileName);
        virtual int closeFile();
        virtual int deleteFile();

        uint8_t read();
        uint8_t peek();

        bool read(uint8_t *buffer, size_t length, size_t bufferSize);

        template <typename T>
        T readAs()
        {
            T result;

            // Ensure there's enough data left
            if (!this->data->getRawData() ||
                (getSize() - filePos) < sizeof(T))
                return result;

            memcpy(&result, data->getRawData() + filePos, sizeof(T));
            filePos += sizeof(T);

            if (filePos >= getSize() && allowWrap)
            {
                filePos = 0; // Wraparound or handle as error?
            }

            return result;
        }

        template <typename T>
        bool readAs(T *buffer, size_t length, size_t bufferSize)
        {
            if (!buffer || !this->data->getRawData())
                return false;

            int unitLen = sizeof(T);

            // Ensure there's enough data left
            if ((getSize() - filePos) < bufferSize)
                return false;

            if (length > bufferSize)
                return false;

            memcpy(&data, this->data->getRawData() + filePos, (unitLen * length));
            filePos += (unitLen * length);

            if (filePos >= getSize())
            {
                if (allowWrap)
                {
                    filePos = 0; // Wraparound or handle as error?
                }
                else
                {
                    return false;
                }
            }

            return true;
        }

        template <typename T>
        T peekAs()
        {
            if (!this->data->getRawData())
                return false;

            T result = 0;

            // Ensure there's enough data left
            if ((getSize() - filePos) < sizeof(T))
                return false;

            memcpy(&result, data->getRawData() + filePos, sizeof(T));

            return result;
        }

        template <typename T>
        bool peekAs(T *buffer, size_t length, size_t bufferSize)
        {
            if (!buffer || !this->data->getRawData())
                return false;

            // Ensure there's enough data left
            if ((getSize() - filePos) < bufferSize)
                return false;

            if (length > bufferSize)
                return false;

            memcpy(&buffer, data->getRawData() + filePos, (bufferSize * length));

            return true;
        }

        DataObject *getRawDataObj() { return data; }
        void setDataObj(DataObject *dObj) { data = dObj; }
        size_t getSize() { return (data ? data->getDataLen() : 0); }

        bool setParam(util::Hash object, size_t param);  // Set a parameter value. Will add a new entry if list has none
        bool getParam(util::Hash object, size_t &param); // Get a parameter value
        uint8_t getParamPos(util::Hash object);          // Find a parameter position in list
        bool deleteParam(util::Hash object);             // Deletes a parameter in the list. Returns false is param was not found

        inline void rewind()
        {
            filePos = 0;
        }

        inline void rewind(size_t byteCount)
        {
            if(byteCount > filePos)
                filePos = 0;
            filePos -= byteCount;
        }


        inline bool setPosition(size_t pos)
        {
            if (pos < getSize())
            {
                filePos = pos;
                return true;
            }
            return false;
        }

        inline size_t getPosition()
        {
            return filePos;
        }

        inline size_t bytesRemaining()
        {
            return (getSize() - filePos);
        }

        inline const char *getFilePath()
        {
            return filePath;
        }
        inline const char *getFileName()
        {
            return fileName;
        }

    private:
        const char *fileName = nullptr;
        const char *filePath = nullptr;

        bool allowWrap = false;
        size_t filePos = 0;
        uint16_t permissions = 0x0000; // If it is ever needed. UNIX style.
        DataObject *data = nullptr;

        FileParameter paramList[paramListSize];
    };

    const char *getFileNamePos(FileObject *fObj);
    util::Hash getFileNameHash(FileObject *fObj);
}