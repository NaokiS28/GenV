/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * services.h - Created on 09-05-2025
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

#include "adminkey.hpp"
#include "genv_sys.hpp"
#include "appmgr.hpp"

#include "video/iface_video.hpp"
#include "audio/iface_audio.hpp"
#include "io/iface_input.hpp"
#include "storage/iface_storage.hpp"
#include "system/iface_system.hpp"

class Services
{
public:
    // Getters
    static Audio::IAudio *getAudio(void) { return s_audio; }
    static Video::IVideo *getVideo(void) { return s_video; }
    static Input::IInput *getInput(void) { return s_input; }
    static Files::IStorage *getStorage(void) { return s_storage; }
    static System::ISystem *getSystem(void) { return s_system; }
    static Apps::AppManager *getAppMgr(void) { return s_app; }

    static inline void setSystem(AdminClass_Key key, System::ISystem *sys) { setSystem(sys); }
    static inline void setVideo(AdminClass_Key key, Video::IVideo *video) { setVideo(video); }
    static inline void setAudio(AdminClass_Key key, Audio::IAudio *audio) { setAudio(audio); }
    static inline void setInput(AdminClass_Key key, Input::IInput *input) { setInput(input); }
    static inline void setStorage(AdminClass_Key key, Files::IStorage *storage) { setStorage(storage); }
    static inline void setAppManager(AdminClass_Key key, Apps::AppManager *app) { setAppManager(app); }

    static inline void destroySystem(AdminClass_Key key) { destroySystem(); }
    static inline void destroyVideo(AdminClass_Key key) { destroyVideo(); }
    static inline void destroyAudio(AdminClass_Key key) { destroyAudio(); }
    static inline void destroyInput(AdminClass_Key key) { destroyInput(); }
    static inline void destroyStorage(AdminClass_Key key) { destroyStorage(); }
    static inline void destroyAppManager(AdminClass_Key key) { destroyAppManager(); }

private:
    static void setSystem(System::ISystem *sys) { s_system = sys; }
    static void setVideo(Video::IVideo *video);
    static void setAudio(Audio::IAudio *audio);
    static void setInput(Input::IInput *input);
    static void setStorage(Files::IStorage *storage);
    static void setAppManager(Apps::AppManager *app) { s_app = app; }

    static void destroySystem();
    static void destroyVideo();
    static void destroyAudio();
    static void destroyInput();
    static void destroyStorage();
    static void destroyAppManager();

    // Static pointers to service implementations
    static Audio::IAudio *s_audio;
    static Video::IVideo *s_video;
    static Input::IInput *s_input;
    static Files::IStorage *s_storage;
    static System::ISystem *s_system;
    static Apps::AppManager *s_app;
};
