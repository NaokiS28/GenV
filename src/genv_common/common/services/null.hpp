/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * null.hpp - Created on 30-05-2025
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

#include "audio.hpp"
#include "input.hpp"
#include "storage.hpp"

namespace Audio {
    class NullAudio : public IAudio {
        NullAudio();
        ~NullAudio();
        bool init() override;
        bool reset() override;
        void shutdown() override;

        bool play(Audio::SoundObject *sObj) override;
        bool stop(Audio::SoundObject *sObj) override;
        bool pause(Audio::SoundObject *sObj) override;
    };
}

namespace Input {
    class NullInput : public IInput {
        NullInput();
        ~NullInput();
        bool init() override;
        bool reset() override;
        void shutdown() override;
    };
}

namespace Files {
    class NullStorage : public IStorage {
        NullStorage();
        ~NullStorage();
        bool init() override;
        bool reset() override;
        void shutdown() override;
    };
}