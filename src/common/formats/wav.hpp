/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * wav.hpp - Created on 06-06-2025
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

#include "riff.hpp"
#include "common/objects/sound.hpp"

class WaveFile : public Audio::SoundObject
{
public:
    WaveFile();
    WaveFile(const char *filepath);
    ~WaveFile() override {};

    int loadSoundFile(const char *filePath) override;

    // Validates Wave header
    static inline bool isWave(Files::FileObject *fObj)
    {
        return RIFFObject::isRIFF(fObj, FCC(fourccWAVE));
    }

    static inline bool isWave(const char *filePath)
    {
        return RIFFObject::isRIFF(filePath, FCC(fourccWAVE));
    }

private:
    struct WaveFormat
    {
        uint32_t blockID = FCC(fourccFMT);
        uint32_t chunkSize = 0;
        uint16_t audioFormat = 0;
        uint16_t numChannels = 0;
        uint32_t sampleRate = 0;
        uint32_t bytesPerSec = 0;
        uint16_t bytesPerBlock = 0;
        uint16_t bitsPerSample = 0;
    } waveFormat;

    struct WaveData
    {
        uint32_t blockID = FCC(fourccDATA);
        uint32_t dataSize = 0;
        uint32_t data = 0;
    } waveData;

    RIFFObject file;

public:
};