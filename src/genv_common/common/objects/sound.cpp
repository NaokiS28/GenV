/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * sound.cpp - Created on 07-06-2025
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

#include "sound.hpp"
#include "common/services/services.hpp"

namespace Audio
{
    bool SoundObject::play() { return Services::getAudio()->play(this); }
    bool SoundObject::stop() { return Services::getAudio()->stop(this); }
    bool SoundObject::pause() { return Services::getAudio()->pause(this); }
    bool SoundObject::isPlaying() { return Services::getAudio()->isPlaying(this); }
    int SoundObject::uploadSample() { return Services::getAudio()->uploadSample(this); }

    SoundObject *createSample(const char* filePath){
        SoundObject *sObj = new SoundObject;
        if (sObj != nullptr)
        {
            if (sObj->loadSoundFile(filePath) == Files::FO_OKAY)
                return sObj;
            else
                delete sObj;
        }
        return nullptr;
    }
}