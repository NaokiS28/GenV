/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * sound.hpp - Created on 27-05-2025
 *
 * DXUX is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * DXUX is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * DXUX. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "file.hpp"

namespace Audio
{
    enum : uint8_t
    {
        SO_OKAY,                    // Success
        SO_ERROR_OUTOFMEMORY,       // Couldn't allocate memory for file
        SO_ERROR_BADPERMISSION,     // Permissions on file denied this action
        SO_ERROR_BADPATH,           // File path was invalid, badly formatted or has invalid characters
        SO_ERROR_BADOBJECT,         // DataObject is bad
        SO_ERROR_BADPARAM,          // A parameter provided did not meet the requirements 
        SO_ERROR_BADNAME,           // Filename is invalid
        SO_ERROR_FILEINUSE,         // File is already open and locked for writing by another process
        SO_ERROR_INVALIDFUNC,       // Function has not been provided on this file provider.
        SO_ERROR_BADFORMAT,         // Format of file is not acceptable by sound engine
        SO_ERROR_UNKNOWN            // Unknown or unexpected error occured
    };

    class SoundObject
    {
    public:
        SoundObject() {}
        virtual ~SoundObject() {}

        virtual int loadSoundFile(const char *path){ return 0; }


    private:
        Files::FileObject file;
    };
}