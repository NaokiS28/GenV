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

#include "video/iface_video.hpp"
#include "audio/iface_audio.hpp"
#include "io/iface_input.hpp"
#include "storage/iface_storage.hpp"
#include "system/iface_system.hpp"

#include "common/services/video/fontman.hpp"
#include "common/services/io/inputman.hpp"
#include "common/services/storage/storeman.hpp"

class Services
{
    friend class GenvSystemClass;

public:
    static inline Audio::IAudio *getAudio(void) { return s_audio; }
    static inline Video::IVideo *getVideo(void) { return s_video; }
    static inline System::ISystem *getSystem(void) { return s_system; }
    static inline Fonts::FontManager *fontManager(void) { return s_fonts; }

    static inline void setSystem(AdminClass_Key key, System::ISystem *sys) { setSystem(sys); }
    static inline void setVideo(AdminClass_Key key, Video::IVideo *video) { setVideo(video); }
    static inline void setAudio(AdminClass_Key key, Audio::IAudio *audio) { setAudio(audio); }

    // static Input::IInput *getInput(void) { return s_input; }
    static Files::IStorage *getStorage(void) { return s_storage; }
    static inline bool addInputDevice(Input::IInput *dev)
    {
        if (!s_input || !dev) return false;
        return s_input->attachDevice(dev); // implement attachDevice(...)
    }
    static inline bool removeInputDevice(Input::IInput *dev)
    {
        if (!s_input || !dev) return false;
        return s_input->detachDevice(dev); // implement detachDevice(...)
    }
    static inline bool addStorageDevice(Files::IStorageDevice *dev)
    {
        if (!s_storage || !dev) return false;
        return s_storage->attachDevice(dev); // implement attachDevice(...)
    }
    static inline bool removeStorageDevice(Files::IStorageDevice *dev)
    {
        if (!s_storage || !dev) return false;
        return s_storage->detachDevice(dev); // implement detachDevice(...)
    }
    static int update();

private:
    static int init();
    static void shutdown();

    // Static pointers to service implementations
    static Audio::IAudio *s_audio;
    static Video::IVideo *s_video;
    static System::ISystem *s_system;
    static void setSystem(System::ISystem *sys);
    static void setVideo(Video::IVideo *video);
    static void setAudio(Audio::IAudio *audio);
    static void destroySystem();
    static void destroyVideo();
    static void destroyAudio();

    static Input::InputManager *s_input;
    static Fonts::FontManager *s_fonts;
    static Files::StorageManager *s_storage;
};