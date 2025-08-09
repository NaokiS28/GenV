/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * common/classes.hpp - Created on 24-04-2025
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

#include "common/objects/sound.hpp"

namespace Audio
{
    enum : uint8_t
    {
        IA_OKAY,                // Success
        IA_ERROR_OUTOFMEMORY,   // Couldn't allocate memory for sample
        IA_ERROR_BADOBJECT,     // SoundObject is invalid
        IA_ERROR_BADPARAM,      // A parameter provided did not meet the requirements
        IA_ERROR_INCOMPATIBLE,  // Sample is incompatible with the target object
        IA_ERROR_BADNAME,       // Sample name is invalid
        IA_ERROR_NOLOCK,        // Could not get lock on audio memory
        IA_ERROR_INVALIDFUNC,   // Function has not been provided on this file provider.
        IA_ERROR_UNKNOWN        // Unknown or unexpected error occured
    };

    class IAudio
    {
    public:
        IAudio() = default;
        virtual ~IAudio() = default;
        virtual bool init() = 0;
        virtual bool reset() = 0;
        virtual void shutdown() = 0;

        virtual bool play(Audio::SoundObject *sObj) = 0;
        virtual bool stop(Audio::SoundObject *sObj) = 0;
        virtual bool pause(Audio::SoundObject *sObj) = 0;
        virtual bool isPlaying(Audio::SoundObject *sObj) = 0;

        virtual Audio::SoundObject *createSample(const char *filePath){
            return Audio::createSample(filePath);
        }
        virtual int uploadSample(Audio::SoundObject *sObj) = 0;
    };
}