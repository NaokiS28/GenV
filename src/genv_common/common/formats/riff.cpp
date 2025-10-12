/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * riff.cpp - Created on 01-06-2025
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

#include "riff.hpp"
#include "common/return_codes.hpp"

// constexpr const util::Hash riffCCHash = "RIFF_CurrentChunkPos"_h;

// Validates RIFF header
bool RIFFObject::isRIFF(Files::FileObject *fObj, uint32_t fcc)
{
    if (fObj == nullptr)
        return false;

    fObj->rewind();
    RIFFHeader hdr = fObj->readAs<RIFFHeader>();
    if (hdr.size >= (sizeof(RIFFHeader) - 8))
    {
        bool result = true;
        if (fcc != fourccNULL)
            result = (hdr.format == fcc);

        return (hdr.magic == FCC(fourccRIFF) &&
                hdr.size == (fObj->getSize() - 8) &&
                result);
    }
    return false;
}

int RIFFObject::openFile(const char *filePath, bool lock)
{
    int result = FileObject::openFile(filePath, lock);
    if (result == GV_OK)
    {
        if (!isRIFF(this, fourccNULL))
        {
            FileObject::closeFile();
            return GV_ERR_INCOMPATIBLE_TYPE;
        }
        FileObject::rewind();
        header = FileObject::readAs<RIFFHeader>();
        return GV_OK;
    }
    return result;
}

bool RIFFObject::skipToChunk(uint32_t fcc)
{
    FileObject::rewind();
    while (FileObject::bytesRemaining() > sizeof(FourCCInt))
    {
        FourCCInt i = FileObject::readAs<FourCCInt>();
        if (i == fcc)
        {
            FileObject::rewind(sizeof(FourCCInt));
            return true;
        }
        FileObject::rewind(sizeof(FourCCInt) - 1);
    }
    return false;
}
