/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * genv_sys.cpp - Created on 09-08-2025
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

#include "genv_sys.hpp"

#include "common/services/storage/iface_storage.hpp"
#include "common/services/storage/storeman.hpp"
#include "services.hpp"
#include "system/iface_system.hpp"
#include "common/logger/log.hpp"
#include "terminal/terminal.h"

#include "video/nullvideo.hpp"
#include "audio/nullaudio.hpp"

#include "hardware.hpp"
#include "app/appmgr.hpp"
#include "common/util/time.hpp"
#include "app/builtin/errorscr/errorscr.hpp"

#ifdef GENV_COMPUTER
#include <stdexcept>
#endif

#define GENV_LOG(fmt, ...) LOG("genv", fmt, __VA_ARGS__)

void GenvSystemClass::startup()
{
    System::ISystem *system = System::makeNewSystem();
    Audio::IAudio *audio = new Audio::NullAudio();
    Video::IVideo *video = new Video::NullVideo();

    int service = Services::init();
    Services::setAudio(adminKey, audio);
    Services::setVideo(adminKey, video);
    Services::setSystem(adminKey, system);

    genv_tty_init(115200);
    GENV_LOG("GenV (" GENV_VERSION ") - Build: " GENV_BUILD);
    GENV_LOG("Genv build date: " GENV_BDATE);
    GENV_LOG("System is: %s %s", system->getSysInfo()->make, system->getSysInfo()->name);
    GENV_LOG("System type: %s", System::getSystemTypeString(system->getSysInfo()->type));
    if (system->getSysInfo()->osname != nullptr)
    {
        GENV_LOG("OS Version: %s", system->getSysInfo()->osname);
    }

    if (service != 0)
    {
        GENV_LOG("Core managers failed to init.");
        halt();
    }
    if (system == nullptr || system->init() != 0)
    {
        GENV_LOG("System manager failed to init.");
        halt();
    }
}

void GenvSystemClass::shutdown()
{
    GENV_LOG("GenV is shutting down...");
    Services::shutdown();
}

void GenvSystemClass::halt(int return_code)
{
    shutdown();
#ifndef GENV_COMPUTER
    GENV_LOG("GenV has halted.");
    while (1)
    {
        __asm__ volatile("");
    }
#else
    throw std::runtime_error("GenV has halted.");
#endif
}