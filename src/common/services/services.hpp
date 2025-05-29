/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * services.h - Created on 09-05-2025
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

#include "video/video.hpp"
#include "audio.hpp"
#include "input.hpp"
#include "appmgr.hpp"
#include "storage.hpp"
#include "system.hpp"

// Not sure I like this setup but it does *work*

class Services
{
public:
    // Getters
    static Audio::IAudio *getAudio() { return s_audio; }
    static Video::IVideo *getVideo() { return s_video; }
    static Input::IInput *getInput() { return s_input; }
    static Files::IStorage *getStorage() { return s_storage; }
    static System::ISystem *getSystem() { return s_system; }

    // AppManager is a static subsystem that orchestrates "apps" running
    static Apps::AppManager &getAppMgr()
    {
        static Apps::AppManager apps;
        return apps;
    }

    // Setters
    static void setAudio(Audio::IAudio *audio) { s_audio = audio; }
    static void setVideo(Video::IVideo *video) { s_video = video; }
    static void setInput(Input::IInput *input) { s_input = input; }
    static void setStorage(Files::IStorage *storage) { s_storage = storage; }
    static void setSystem(System::ISystem *sys) { s_system = sys; }

    static bool startup();
    static void shutdown();

    static size_t millis();     // Forwarder function to system millis with protection

private:
    // Static pointers to service implementations
    static Audio::IAudio *s_audio;
    static Video::IVideo *s_video;
    static Input::IInput *s_input;
    static Files::IStorage *s_storage;
    static System::ISystem *s_system;
};