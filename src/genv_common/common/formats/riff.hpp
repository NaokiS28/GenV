/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * riff.hpp - Created on 01-06-2025
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
#include <stdint.h>
#include "common/objects/file.hpp"
#include "common/util/hash.hpp"
#include "common/util/templates.hpp"
#include "common/return_codes.hpp"

using FourCC = uint32_t;
constexpr FourCC fccRIFF = "RIFF"_c;
constexpr FourCC fccDATA = "data"_c;
constexpr FourCC fccFMT = "fmt "_c;
constexpr FourCC fccWAVE = "WAVE"_c;
constexpr FourCC fccXWMA = "XWMA"_c;
constexpr FourCC fccDPDS = "dpds"_c;
constexpr FourCC fccNULL = UINT32_MAX;

struct RIFFHeader
{
    uint32_t magic = fccRIFF;
    uint32_t size = 0;
    uint32_t format = 0;
};

class RIFFObject : public Files::FileObject
{
public:
    RIFFObject() : Files::FileObject("RIFF_Test"_h) {}
    virtual int openFile(const char *filePath, bool lock = false);

    // Validates RIFF header
    static bool isRIFF(Files::FileObject *fObj, uint32_t fcc);
    static bool isRIFF(const char *filePath, uint32_t fcc)
    {
        Files::FileObject file("RIFF_Test"_h);
        if (file.openFile(filePath) == GV_OK)
        {
            bool r = isRIFF(&file, fcc);
            file.closeFile();
            return r;
        }
        file.closeFile();
        return false;
    }

    bool skipToChunk(uint32_t fcc);

protected:
    RIFFHeader header = {0};
};