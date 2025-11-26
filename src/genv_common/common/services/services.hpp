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
#include "common/util/templates.hpp"
#include "genv_sys.hpp"

#include "video/iface_video.hpp"
#include "audio/iface_audio.hpp"
#include "io/iface_input.hpp"
#include "storage/iface_storage.hpp"
#include "system/iface_system.hpp"

#include "common/services/video/fontman.hpp"
#include "common/services/io/inputman.hpp"
#include "common/services/storage/storeman.hpp"

// Services uses pointers to avoid issues with atexit() as not all platforms implement this.
// Using pointers allows the services class to be entirely static. We do this so there's no
// need for functions or classes to have to find the existing resource. We also do it this
// way because of the atexit issue, as a static function to return a static instance crashes
// on bare-metal platforms like PS1. This is the same reason why all the managers are pointers,
// so we control the lifecycle to avoid this.

typedef void (*AsyncServiceFunction)(void *arg);
struct AsyncService
{
    const char *name          = nullptr;
    AsyncServiceFunction func = nullptr;
    void *arg                 = nullptr;
    int listID                = -1;
};

class Services
{
    friend class GenvSystemClass;

public:
    static inline Audio::IAudio *getAudio(void) { return s_audio; }
    static inline Video::IVideo *getVideo(void) { return s_video; }
    static inline System::ISystem *getSystem(void) { return s_system; }
    static inline Fonts::FontManager *fontManager(void) { return s_fonts; }
    static inline Input::InputManager *inputManager(void) { return s_input; }

    static inline void setSystem(AdminClass_Key key, System::ISystem *sys) { setSystem(sys); }
    static inline void setVideo(AdminClass_Key key, Video::IVideo *video) { setVideo(video); }
    static inline void setAudio(AdminClass_Key key, Audio::IAudio *audio) { setAudio(audio); }

    // static Input::IInputDriver *getInput(void) { return s_input; }
    static Files::IStorage *getStorage(void) { return s_storage; }
    static inline bool registerInputDriver(Input::IInputDriver *dev)
    {
        if (!s_input || !dev) return false;
        return s_input->registerDriver(dev);
    }
    static inline bool unregisterInputDriver(Input::IInputDriver *dev)
    {
        if (!s_input || !dev) return false;
        return s_input->unregisterDriver(dev);
    }

    static inline bool attachInputDevice(Input::IInputDevice *dev)
    {
        if (!s_input || !dev) return false;
        return s_input->attachDevice(dev);
    }
    static inline bool dettachInputDevice(Input::IInputDevice *dev)
    {
        if (!s_input || !dev) return false;
        return s_input->detachDevice(dev);
    }

    static inline bool registerStorageDriver(Files::IStorageDriver *dev)
    {
        if (!s_storage || !dev) return false;
        return s_storage->registerDriver(dev);
    }
    static inline bool unregisterStorageDriver(Files::IStorageDriver *dev)
    {
        if (!s_storage || !dev) return false;
        return s_storage->unregisterDriver(dev);
    }

    static inline bool addStorageDevice(Files::IStorageDevice *dev)
    {
        if (!s_storage || !dev) return false;
        return s_storage->attachDevice(dev);
    }
    static inline bool removeStorageDevice(Files::IStorageDevice *dev)
    {
        if (!s_storage || !dev) return false;
        return s_storage->detachDevice(dev);
    }
    static int update();
    static int updateAsyncServices(); // This runs until either vsync occurs or all services are finished

    static inline size_t gfx_size(size_t size) { return (s_video ? s_video->getBufferSize(size) : 0); }
    static inline void *gfx_alloc(size_t size) { return (s_video ? s_video->allocate(size) : nullptr); }

    static int registerAsyncService(AsyncService &service, void *arg);
    static int unregisterAsyncService(AsyncService &service);

private:
    static int createManagers();
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
    static util::PointerList<AsyncService *, 10> s_service;
};