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

namespace Files
{

    enum : uint8_t
    {
        FO_OKAY,                    // Success
        FO_ERROR_OUTOFMEMORY,       // Couldn't allocate memory for file
        FO_ERROR_BADPERMISSION,     // Permissions on file denied this action
        FO_ERROR_BADPATH,           // File path was invalid, badly formatted or has invalid characters
        FO_ERROR_BADOBJECT,         // DataObject is bad
        FO_ERROR_BADPARAM,          // A parameter provided did not meet the requirements 
        FO_ERROR_BADNAME,           // Filename is invalid
        FO_ERROR_FILEINUSE,         // File is already open and locked for writing
        FO_ERROR_NOLOCK,            // File is not locked or lock could not be set
        FO_ERROR_INVALIDFUNC,       // Function has not been provided on this file provider.
        FO_ERROR_UNKNOWN            // Unknown or unexpected error occured
    };

    class FileObject
    {
    public:
        FileObject() {}
        virtual ~FileObject() {
            if(fileName) delete fileName;
            if(filePath) delete filePath;
            closeFile();
        }

        virtual int openFile(bool lock = false) { return 0; } // Open file, optionally lock for writing else will be opened as read-only
        virtual int saveFile() { return 0; }
        virtual int renameFile(const char *filename){ return 0; }
        virtual int closeFile() { return 0; }
        virtual int deleteFile() { return 0; }

        DataObject data;

    private:
        const char *fileName = nullptr;
        const char *filePath = nullptr;
        size_t fileSize = 0;                // File size in bytea
        uint16_t permissions = 0x0000;      // If it is ever needed. UNIX style.
    };
}