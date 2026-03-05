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
#include "common/services/system/iface_coroutine.hpp"
#include "common/util/templates.hpp"
#include "genv_sys.hpp"

#include "video/iface_video.hpp"
#include "audio/iface_audio.hpp"
#include "io/iface_input.hpp"
#include "system/iface_system.hpp"

#include "common/services/video/fontman.hpp"
#include "common/services/io/playermgr.hpp"
#include "common/services/io/devicemgr.hpp"
#include "common/services/io/outputmgr.hpp"
#include "common/services/storage/storemgr.hpp"

// Services uses pointers to avoid issues with atexit() as not all platforms implement this.
// Using pointers allows the services class to be entirely static. We do this so there's no
// need for functions or classes to have to find the existing resource. We also do it this
// way because of the atexit issue, as a  function to return a static instance crashes
// on bare-metal platforms like PS1. This is the same reason why all the managers are pointers,
// so we control the lifecycle to avoid this.

class ServiceManager
{
    friend class GenvSystemClass;

public:
    ServiceManager() {}
    ~ServiceManager() { shutdown(); }

    inline Audio::IAudio *getAudio(void) { return s_audio; }
    inline Video::IVideo *getVideo(void) { return s_video; }
    inline System::ISystem *getSystem(void) { return s_system; }
    inline Files::StorageManager *getStorage(void) { return s_storage; }
    inline Fonts::FontManager *getFontManager(void) { return s_fonts; }
    inline IO::PlayerManager  *getPlayerManager(void)  { return s_playerManager; }
    inline IO::DeviceManager  *getDeviceManager(void)  { return s_deviceManager; }
    inline IO::OutputManager  *getOutputManager(void)  { return s_outputManager; }

    inline void setSystem(AdminClass_Key key, System::ISystem *sys) { setSystem(sys); }
    inline void setVideo(AdminClass_Key key, Video::IVideo *video) { setVideo(video); }
    inline void setAudio(AdminClass_Key key, Audio::IAudio *audio) { setAudio(audio); }

    inline bool registerDriver(IO::IDriver *dev)
    {
        if (!s_deviceManager || !dev) return false;
        return s_deviceManager->registerDriver(dev);
    }

    inline bool unregisterDriver(IO::IDriver *dev)
    {
        if (!s_deviceManager || !dev) return false;
        return s_deviceManager->unregisterDriver(dev);
    }

    int update();
    int updateCoroutines(); // This runs until either vsync occurs or all services are finished
    int registerCoroutine(ICoroutine &coroutine);

private:
    int createManagers();
    int init();
    void shutdown();

    void setSystem(System::ISystem *sys);
    void setVideo(Video::IVideo *video);
    void setAudio(Audio::IAudio *audio);

    Audio::IAudio *s_audio             = nullptr;
    Video::IVideo *s_video             = nullptr;
    System::ISystem *s_system          = nullptr;
    IO::PlayerManager *s_playerManager  = nullptr;
    IO::DeviceManager *s_deviceManager  = nullptr;
    IO::OutputManager *s_outputManager  = nullptr;
    Fonts::FontManager *s_fonts        = nullptr;
    Files::StorageManager *s_storage   = nullptr;
    util::PointerList<ICoroutine *, 10> s_coroutines;
};

ServiceManager *getServiceManager();

inline System::ISystem *getSystem()
{
    return getServiceManager()->getSystem();
}