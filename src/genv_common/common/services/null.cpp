/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * null.cpp - Created on 30-05-2025
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

#include "null.hpp"

namespace Audio
{
    NullAudio::NullAudio() {}
    NullAudio::~NullAudio() {}
    bool NullAudio::init() { return true; }
    bool NullAudio::reset() { return true; }
    void NullAudio::shutdown() { return; }
    bool NullAudio::play(Audio::SoundObject *sObj) { return false; }
    bool NullAudio::stop(Audio::SoundObject *sObj) { return false; }
    bool NullAudio::pause(Audio::SoundObject *sObj) { return false; }
}

namespace Input
{
    NullInput::NullInput() {}
    NullInput::~NullInput() {}
    bool NullInput::init() { return true; }
    bool NullInput::reset() { return true; }
    void NullInput::shutdown() { return; }
}

namespace Files
{
    NullStorage::NullStorage() {}
    NullStorage::~NullStorage() {}
    bool NullStorage::init() { return true; }
    bool NullStorage::reset() { return true; }
    void NullStorage::shutdown() { return; }
}