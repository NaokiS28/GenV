/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * sound.hpp - Created on 25-05-2025
 *
 * DXUX is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * DXUX is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * DXUX. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "common/services/audio.hpp"
#include "../window-mgr.hpp"

using namespace Audio;

class IWinAudio : public IAudio
{
public:
    virtual void setWindow(WindowObject *hWnd) = 0; // We have to do this on windows. Urgh.
};

class WinAudio : public IWinAudio
{
private:
    WindowObject *gpuWnd = nullptr;
    IWinAudio *audioDriver = nullptr;

public:
    WinAudio();
    ~WinAudio();
    bool init();
    void setWindow(WindowObject *hWnd){
        if(hWnd)
            gpuWnd = hWnd;
    }
    inline bool reset() { return audioDriver ? audioDriver->reset() : false; }
    inline void shutdown()
    {
        if (audioDriver)
            audioDriver->shutdown();
    }

    IAudio *getAudioService() { return audioDriver; }

};
