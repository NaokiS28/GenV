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

#include "common/return_codes.hpp"
#include "common/util/hash.hpp"
#include "data.hpp"
#include "object.hpp"

namespace Files
{
    static constexpr const char *GENV_FILE_OBJ_TYPENAME = "GenVFileObject";

    class FileObject : public ObjectBase, public DataObject
    {

    public:
        FileObject(util::Hash objectID) : ObjectBase(objectID)
        {
            setObjectType(GENV_FILE_OBJ_TYPENAME);
        }
        FileObject(util::Hash objectID, const char *filePath, bool lock = false) : ObjectBase(objectID)
        {
            openFile(filePath, lock);
            setObjectType(GENV_FILE_OBJ_TYPENAME);
        }
        FileObject(util::Hash objectID, const uint8_t *data, size_t length) : ObjectBase(objectID), DataObject(data, length)
        {
            setObjectType(GENV_FILE_OBJ_TYPENAME);
        }
        virtual ~FileObject();

        // Open file, optionally lock for writing else will be opened as read-only
        virtual int openFile(const char *filePath, bool lock = false);
        virtual int saveFile();
        virtual int renameFile(const char *fileName);
        virtual int closeFile();
        virtual int deleteFile();

        const uint8_t *getRawData() { return DataObject::getRawData(); }

        inline const char *const getFilePath()
        {
            return filePath;
        }
        inline const char *const getFileName()
        {
            return fileName;
        }

    private:
        const char *fileName = nullptr;
        const char *filePath = nullptr;

        uint16_t permissions = 0x0000; // If it is ever needed. UNIX style.
    };

    const char *getFileNamePos(FileObject *fObj);
    util::Hash getFileNameHash(FileObject *fObj);
} // namespace Files