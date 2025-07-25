/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * soundfile.cpp - Created on 07-06-2025
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

#include "soundfile.hpp"
#include <cstring>
#include "wav.hpp"

namespace Audio
{
    enum class ecAudioFormat {
        AF_WAVE,
        AF_END
    };

    struct AudioFileFormat
    {
        const char ext[8];
        ecAudioFormat format;
    };

    constexpr const AudioFileFormat afWaveFile = {".wav", ecAudioFormat::AF_WAVE};
    constexpr const AudioFileFormat AudioFormatList[] = {
        afWaveFile
    };

    Audio::SoundObject *openAudioFile(const char *filePath, size_t pathLen)
    {
        if(filePath == nullptr || pathLen == 0)
            return nullptr;

        Audio::SoundObject *sObj = nullptr;
        
        for(auto &af : AudioFormatList){
            const char *pos = std::strstr(filePath, af.ext);
            if(pos != nullptr){
                switch (af.format){
                    case ecAudioFormat::AF_WAVE:
                        sObj = new WaveFile(filePath);
                        break;
                    default:
                        break;
                }
            }
        }

        return sObj;
    }
}