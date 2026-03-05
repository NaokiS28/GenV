/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * services.cpp - Created on 09-05-2025
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

#include "services.hpp"
#include "common/logger/log.hpp"

#include "common/return_codes.hpp"
#include "common/services/adminkey.hpp"
#include "common/services/io/playermgr.hpp"
#include "common/services/io/devicemgr.hpp"
#include "common/services/perfmon.hpp"
#include "common/services/storage/storemgr.hpp"
#include "common/services/system/system.hpp"

#define LOG_SVC(fmt, ...) LOG("services", fmt __VA_OPT__(, ) __VA_ARGS__)

constexpr const char szManagerNullptr[]     = "Failed to create %s.";
constexpr const char szManagerInitError[]   = "Failed to init %s, reported error code: %i";
constexpr const char szChangeServiceError[] = "Failed to change to new %s: %i";
constexpr const char szUnkownError[]        = "Unknown error reported from %s: %i";

constexpr const char szSystem[]        = "system service";
constexpr const char szVideo[]         = "video service";
constexpr const char szAudio[]         = "audio service";
constexpr const char szStorage[]       = "storage manager";
constexpr const char szPlayerManager[] = "player manager";
constexpr const char szDevice[]        = "devicer manager";
constexpr const char szFont[]          = "font manager";
constexpr const char szUnknown[]       = "unknown";

enum ServiceError
{
    SE_NULLPTR,
    SE_INIT_FAILED
};

enum ServiceName
{
    SN_SYSTEM,
    SN_VIDEO,
    SN_AUDIO,
    SN_STORAGE,
    SN_PLAYER,
    SN_DEVICE,
    SN_FONT
};

constexpr int makeErrorCode(ServiceName sn, ServiceError se)
{
    return ((sn << 8) | (se & 0xFF));
}

constexpr int getServiceID(int errorcode)
{
    return (errorcode >> 8);
}

constexpr int getErrorCode(int errorcode)
{
    return (errorcode & 0xFF);
}

constexpr const char *szServiceName(int errorcode)
{
    switch (getServiceID(errorcode))
    {
    case SN_SYSTEM: return szSystem;
    case SN_VIDEO: return szVideo;
    case SN_AUDIO: return szAudio;
    case SN_STORAGE: return szStorage;
    case SN_PLAYER: return szPlayerManager;
    case SN_DEVICE: return szDevice;
    case SN_FONT: return szFont;
    default: return szUnknown;
    }
}

constexpr const char *szGetErrorString(int errorcode)
{
    switch (getErrorCode(errorcode))
    {
    case SE_NULLPTR: return szManagerNullptr;
    case SE_INIT_FAILED: return szManagerInitError;
    default: return szUnkownError;
    }
}

template <typename T>
static void destroyService(T *&ptr)
{
    if (!ptr) return;
    ptr->shutdown();
    delete ptr;
    ptr = nullptr;
}

int ServiceManager::createManagers()
{
    int error = 0;
    s_storage = new Files::StorageManager(AdminClass_Key());
    if (!s_storage) error = makeErrorCode(SN_STORAGE, SE_NULLPTR);

    if (!error)
    {
        s_playerManager = new IO::PlayerManager();
        if (!s_playerManager) error = makeErrorCode(SN_PLAYER, SE_NULLPTR);
    }

    if (!error)
    {
        s_outputManager = new IO::OutputManager();
        if (!s_outputManager) error = makeErrorCode(SN_PLAYER, SE_NULLPTR);
    }

    if (!error)
    {
        s_deviceManager = new IO::DeviceManager(AdminClass_Key(), *s_playerManager);
        if (!s_deviceManager) error = makeErrorCode(SN_DEVICE, SE_NULLPTR);
    }

    if (!error)
    {
        s_fonts = new Fonts::FontManager(AdminClass_Key());
        if (!s_fonts) error = makeErrorCode(SN_FONT, SE_NULLPTR);
    }

    if (error)
        LOG("services",
            szGetErrorString(error),
            szServiceName(error),
            getErrorCode(error));
    return error;
}

int ServiceManager::init()
{
    int error = 0;
    if (s_storage->init() != GV_OK) error = makeErrorCode(SN_STORAGE, SE_INIT_FAILED);
    if (s_deviceManager->init() != GV_OK) error = makeErrorCode(SN_DEVICE, SE_INIT_FAILED);
    if (s_fonts->init() != GV_OK) error = makeErrorCode(SN_FONT, SE_INIT_FAILED);
    return error;
}

int ServiceManager::update()
{
    int r = System::SM_NORMAL;
    r     = s_system->update();
    System::PerfMon.finishSystemExec();

    // It's unlikely these would ever be null at this point
    // if (s_storage)
    //{
    // s_storage->update();
    // System::PerfMon.finishStorageUpdate();
    //}
    if (s_deviceManager)
    {
        s_deviceManager->update();
        System::PerfMon.finishInputUpdate();
    }
    return r;
}

void ServiceManager::shutdown()
{
    LOG_SVC("Shutting down ServiceManager.");
    destroyService(s_audio);
    destroyService(s_video);
    destroyService(s_system);

    if (s_deviceManager)
    {
        s_deviceManager->shutdown();
        delete s_deviceManager;
        s_deviceManager = nullptr;
    }
    if (s_outputManager)
    {
        delete s_outputManager;
        s_outputManager = nullptr;
    }
    if (s_playerManager)
    {
        delete s_playerManager;
        s_playerManager = nullptr;
    }
    if (s_storage)
    {
        s_storage->shutdown();
        delete s_storage;
        s_storage = nullptr;
    }
    if (s_fonts)
    {
        s_fonts->shutdown();
        delete s_fonts;
        s_fonts = nullptr;
    }
}

void ServiceManager::setSystem(System::ISystem *system)
{
    if (!system)
    {
        LOG_SVC(szChangeServiceError, szSystem, SE_NULLPTR);
        return;
    }
    destroyService(s_system);
    s_system = system;
}

void ServiceManager::setVideo(Video::IVideo *video)
{
    if (!video)
    {
        LOG_SVC(szChangeServiceError, szVideo, SE_NULLPTR);
        return;
    }
    destroyService(s_video);
    s_video = video;
}

void ServiceManager::setAudio(Audio::IAudio *audio)
{
    if (!audio)
    {
        LOG_SVC(szChangeServiceError, szAudio, SE_NULLPTR);
        return;
    }
    destroyService(s_audio);
    s_audio = audio;
}

int ServiceManager::updateCoroutines()
{
    for (auto coroutine : s_coroutines)
    {
        if (s_video->waitingForVSync()) // Only run updates whilst waiting for vsync.
            coroutine->resume();
        else
            return GV_OK;
    }
    return 1;
}

int ServiceManager::registerCoroutine(ICoroutine &coroutine)
{
    if (s_coroutines.ready())
    {
        auto position = s_coroutines.append(&coroutine);
        if (position == -1)
            LOG_SVC("Could not register coroutine %s: Unknown error occured.", coroutine.name());

        coroutine.listID = position;
        // service.arg = arg;
        LOG_SVC("Registered %s service.", coroutine.name());
        return GV_OK;
    }
    LOG_SVC("Could not register coroutine %s: Internal list error.", coroutine.name());
    return 1;
}

// --- IO free functions ---------------------------------------------------------

namespace IO
{
    PlayerManager *playerManager()
    {
        return getServiceManager()->getPlayerManager();
    }

    PlayerView player(Player p)
    {
        return PlayerView(getServiceManager()->getPlayerManager(), p);
    }

    PlayerView arcade()
    {
        return PlayerView(getServiceManager()->getPlayerManager(), Player::ARCADE_CABINET);
    }

    OutputManager *outputManager()
    {
        return getServiceManager()->getOutputManager();
    }
} // namespace IO

// --- ServiceManager ------------------------------------------------------------

static ServiceManager *serviceManager = nullptr;
ServiceManager *getServiceManager()
{
    if (!serviceManager)
    {
        serviceManager = new ServiceManager;
        if (!serviceManager)
        {
            LOG("services", "Failed to create ServiceManager.");
            return nullptr;
        }
    }
    return serviceManager;
}