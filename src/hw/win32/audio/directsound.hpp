/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * directsound.hpp - Created on 25-05-2025
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

#include <dsound.h>
#include "audio.hpp"

#include "common/objects/sound.hpp"

class DirectSound : public IWinAudio
{
private:
    WindowObject *gpuWnd;

    LPDIRECTSOUND8          dsdev = nullptr;
    LPDIRECTSOUNDBUFFER     dsbuffer = nullptr;

public:
    DirectSound(WindowObject *hWnd);
    ~DirectSound();

    void setWindow(WindowObject *hWnd);
    bool init();
    bool reset();
    void shutdown();

    
    bool LoadWavFile(const char* filename);
};