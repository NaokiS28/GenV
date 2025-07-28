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

using FourCC = const char;
using FourCCInt = uint32_t;
constexpr FourCC fourccRIFF[] = "RIFF";
constexpr FourCC fourccDATA[] = "data";
constexpr FourCC fourccFMT[] = "fmt ";
constexpr FourCC fourccWAVE[] = "WAVE";
constexpr FourCC fourccXWMA[] = "XWMA";
constexpr FourCC fourccDPDS[] = "dpds";

constexpr const uint32_t fourccNULL = UINT32_MAX;

static constexpr inline uint32_t fccToUInt(FourCC* str, int len){
    if(len >= 4)
        return (str[0] | str[1] << 8 | str[2] << 16 | str[3] << 24);
    else
        return 0;
}

#define FCC(str) fccToUInt(str, sizeof(str))

struct RIFFHeader
{
    uint32_t magic = FCC(fourccRIFF);
    uint32_t size = 0;
    uint32_t format = 0;
};


class RIFFObject : public Files::FileObject {
    public:
        virtual int openFile(const char *filePath, bool lock = false);

        // Validates RIFF header
        static bool isRIFF(Files::FileObject *fObj, uint32_t fcc);
        static bool isRIFF(const char *filePath, uint32_t fcc){
            Files::FileObject file;
            if(file.openFile(filePath) == Files::FO_OKAY){
                bool r = isRIFF(&file, fcc);
                file.closeFile();
                return r;
            }
            file.closeFile();
            return false;
        }

        bool skipToChunk(uint32_t fcc);
    
    protected:
        RIFFHeader header = { 0 };
};