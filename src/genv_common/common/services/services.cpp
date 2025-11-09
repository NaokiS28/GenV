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
#include "common/services/io/inputman.hpp"
#include "common/services/perfmon.hpp"
#include "common/services/storage/storeman.hpp"
#include "common/services/system/iface_system.hpp"
#include "hardware.hpp"

// Static member definitions
Audio::IAudio *Services::s_audio = nullptr;
Video::IVideo *Services::s_video = nullptr;
System::ISystem *Services::s_system = nullptr;

Input::InputManager *Services::s_input = nullptr;
Files::StorageManager *Services::s_storage = nullptr;
Fonts::FontManager *Services::s_fonts = nullptr;

const char *szManagerNullptr = "Failed to create %s.";
const char *szManagerInitError = "Failed to init %s, reported error code: %i";
const char *szChangeServiceError = "Failed to change to new %s: %i";
const char *szUnkownError = "Unknown error reported from %s: %i";

const char *szSystem = "system service";
const char *szVideo = "video service";
const char *szAudio = "audio service";
const char *szStorage = "storage manager";
const char *szInput = "input manager";
const char *szFont = "font manager";
const char *szUnknown = "unknown";

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
    SN_INPUT,
    SN_FONT
};

constexpr const int makeErrorCode(ServiceName sn, ServiceError se)
{
    return ((sn << 8) | (se & 0xFF));
}

constexpr const int getServiceID(int errorcode)
{
    return (errorcode >> 4);
}

constexpr const int getErrorCode(int errorcode)
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
    case SN_INPUT: return szInput;
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

int Services::createManagers()
{
    int error = 0;
    s_storage = new Files::StorageManager(AdminClass_Key());
    if (!s_storage) error = makeErrorCode(SN_STORAGE, SE_NULLPTR);

    if (!error)
    {
        s_input = new Input::InputManager(AdminClass_Key());
        if (!s_input) error = makeErrorCode(SN_INPUT, SE_NULLPTR);
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

int Services::init()
{
    int error = 0;
    if (s_storage->init() != GV_OK) error = makeErrorCode(SN_STORAGE, SE_INIT_FAILED);
    if (s_input->init() != GV_OK) error = makeErrorCode(SN_INPUT, SE_INIT_FAILED);
    if (s_fonts->init() != GV_OK) error = makeErrorCode(SN_FONT, SE_INIT_FAILED);
    return error;
}

int Services::update()
{
    int r = System::SM_NORMAL;
    r = s_system->update();
    System::PerfMon.finishSystemExec();

    // It's unlikely these would ever be null at this point
    if (s_storage)
    {
        s_storage->update();
        System::PerfMon.finishStorageUpdate();
    }
    if (s_input)
    {
        s_input->update();
        System::PerfMon.finishInputUpdate();
    }
    return r;
}

void Services::shutdown()
{
    LOG("services", "Shutting down services.");
    destroyAudio();
    destroyVideo();
    destroySystem();

    if (s_input)
    {
        s_input->shutdown();
        delete s_input;
        s_input = nullptr;
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

void Services::setSystem(System::ISystem *system)
{
    if (!system)
    {
        int error = makeErrorCode(SN_SYSTEM, SE_NULLPTR);
        LOG("services", szChangeServiceError,
            szServiceName(error),
            getErrorCode(error));
        return;
    }

    if (s_system)
    {
        s_system->shutdown();
        delete s_system;
    }

    s_system = system;
}

void Services::setVideo(Video::IVideo *video)
{
    if (!video)
    {
        int error = makeErrorCode(SN_VIDEO, SE_NULLPTR);
        LOG("services", szChangeServiceError,
            szServiceName(error),
            getErrorCode(error));
        return;
    }

    if (s_video)
    {
        s_video->shutdown();
        delete s_video;
    }

    s_video = video;
}

void Services::setAudio(Audio::IAudio *audio)
{
    if (!audio)
    {
        int error = makeErrorCode(SN_AUDIO, SE_NULLPTR);
        LOG("services", szChangeServiceError,
            szServiceName(error),
            getErrorCode(error));
        return;
    }
    if (s_audio)
    {
        s_audio->shutdown();
        delete s_audio;
    }

    s_audio = audio;
}

void Services::destroySystem()
{
    if (!s_system)
        return;
    s_system->shutdown();
    delete s_system;
    s_system = nullptr;
}

void Services::destroyVideo()
{
    if (!s_video)
        return;
    s_video->shutdown();
    delete s_video;
    s_video = nullptr;
}

void Services::destroyAudio()
{
    if (!s_audio)
        return;
    s_audio->shutdown();
    delete s_audio;
    s_audio = nullptr;
}
