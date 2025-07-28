/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * SPUSound.hpp - Created on 25-05-2025
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

#include "audio.hpp"

bool SPUSound::play(Audio::SoundObject *sObj)
{
    
    return false;
}

bool SPUSound::stop(Audio::SoundObject *sObj)
{
    
    return false;
}

bool SPUSound::pause(Audio::SoundObject *sObj)
{
    
    return false;
}

bool SPUSound::isPlaying(Audio::SoundObject *sObj)
{
    
    return false;
}

int SPUSound::uploadSample(Audio::SoundObject *sObj)
{
    
    return Audio::IA_OKAY;
}

SPUSound::SPUSound()
{
}

SPUSound::~SPUSound()
{
    shutdown();
}

bool SPUSound::init()
{
    return true;
}

bool SPUSound::reset()
{
    return true;
}

void SPUSound::shutdown()
{
    
}