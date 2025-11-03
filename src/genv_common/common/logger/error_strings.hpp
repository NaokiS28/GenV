/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * error_strings.cpp - Created on 03-10-2025
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
#include <stdarg.h>
#include <stdint.h>

// Base case: no more expressions
constexpr uint8_t testParams()
{
    return 0;
}

// Param check is a recursive parameter test which returns which param number is not true
// or 0 if all parameters are true.
template <typename... Rest>
constexpr uint8_t testParams(bool first, Rest... rest)
{
    if (first)
    {
        uint8_t p = testParams(rest...);
        if (p == UINT8_MAX) return 0;
        return p;
    }
    else
        return 1;
}

namespace LogErrorStrings
{
    constexpr const char GenvCreatedItem[] = "Created new %s: %s (%s)";
    constexpr const char GenvLoadedItem[] = "Loaded new %s: %s (%s)";
    constexpr const char GenvRemovedItem[] = "Removed %s: %s (%s)";
    constexpr const char GenvCreateItemFailed[] = "Could not create %s: %s (%s)";
    constexpr const char GenvLoadItemFailed[] = "Could not load %s: %s (%s)";
    constexpr const char GenvRemoveItemFailed[] = "Could not remove %s: %s (%s)";
} // namespace LogErrorStrings

namespace LogReasonStrings
{
    constexpr const char GenvBadObject[] = "Object is invalid.";
    constexpr const char GenvParameterError[] = "Parameter error.";
    constexpr const char GenvItemNotFound[] = "Item not found.";
    constexpr const char GenvOutOfMemory[] = "Out of memory.";
    constexpr const char GenvListIsFull[] = "List is full.";
    constexpr const char GenvListIsEmpty[] = "List is empty.";
    constexpr const char GenvTypeIncompatible[] = "Type is incompatbile.";
    constexpr const char GenvUploadFailed[] = "Upload failed.";
    constexpr const char GenvDownloadFailed[] = "Download failed.";
} // namespace LogReasonStrings

namespace ObjectStrings
{
    constexpr const char TextureObject[] = "Texture Object";
    constexpr const char FileObject[] = "File Object";
    constexpr const char FontsetObject[] = "Fontset";
    constexpr const char FontObject[] = "Font Object";
    constexpr const char SoundObject[] = "Sound Object";
    constexpr const char DataObject[] = "Data Object";
} // namespace ObjectStrings