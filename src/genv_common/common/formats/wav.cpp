/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * wav.cpp - Created on 06-06-2025
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

#include "wav.hpp"
#include "common/objects/file.hpp"

WaveFile::WaveFile() : Audio::SoundObject(&file)
{
}

WaveFile::WaveFile(const char *filepath) : Audio::SoundObject(&file)
{
    loadSoundFile(filepath);
}

int WaveFile::loadSoundFile(const char *filePath)
{
    SoundObject::valid = ((file.openFile(filePath) == Files::FO_OKAY) &&
                          WaveFile::isWave(&file));

    if (valid)
    {
        file.rewind();
        file.readAs<RIFFHeader>();
        file.skipToChunk(FCC(fourccFMT));
        waveFormat = file.readAs<WaveFormat>();
        file.skipToChunk(FCC(fourccDATA));
        waveData.blockID = file.readAs<uint32_t>();
        waveData.dataSize = file.readAs<uint32_t>();
        waveData.data = file.getPosition();

        SoundObject::meta.bitDepth = waveFormat.bitsPerSample;
        SoundObject::meta.channels = waveFormat.numChannels;
        SoundObject::meta.sampleRate = waveFormat.sampleRate;
        SoundObject::meta.sampleLength = waveData.dataSize;
        SoundObject::sampleData = (file.getRawDataObj()->getRawData() + waveData.data);  // TODO: Change from being hardcoded

        return Audio::SO_OKAY;
    }

    return Audio::SO_ERROR_INCOMPATIBLE;
}
