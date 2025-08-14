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

#include "services.hpp"
#include "system/iface_system.hpp"
#include "common/logger/log.hpp"
#include "terminal/terminal.h"

#include "video/nullvideo.hpp"
#include "audio/nullaudio.hpp"
#include "io/nullinput.hpp"
#include "storage/nullstorage.hpp"

#include "hardware.hpp"
#include "app/appmgr.hpp"
#include "app/builtin/errorscr/errorscr.hpp"

#ifdef GENV_COMPUTER
#include <stdexcept>
#endif

#define GENV_LOG(fmt, ...) LOG("genv", fmt __VA_OPT__(, ) __VA_ARGS__)

void GenvSystemClass::startup()
{
    // Pointer for system and reference for app manager. Again, this seems... not right.
    // Works... but not right. Context: This stuff was moved into here but was in main.
    System::ISystem *system = System::makeNewSystem();
    Audio::IAudio *audio = new Audio::NullAudio();
    Video::IVideo *video = new Video::NullVideo();
    Input::IInput *input = new Input::NullInput();
    Files::IStorage *storage = new Files::NullStorage();

    genv_tty_init(115200);
    GENV_LOG("GenV starting up...");

    Services::setStorage(adminKey, storage);
    Services::setInput(adminKey, input);
    Services::setAudio(adminKey,audio);
    Services::setVideo(adminKey, video);
    Services::setSystem(adminKey, system);

    if (system == nullptr || system->init() != 0)
    { // Always init system manager first.
        GENV_LOG("System manager failed to init.");
        halt();
    }
}

void GenvSystemClass::shutdown()
{
    GENV_LOG("GenV is shutting down...");
    Services::destroyStorage(adminKey);
    Services::destroyInput(adminKey);
    Services::destroyAudio(adminKey);
    Services::destroyVideo(adminKey);
    Services::destroySystem(adminKey);
}

void GenvSystemClass::halt(int return_code)
{
    shutdown();
#ifndef GENV_COMPUTER
    GENV_LOG("GenV has halted.");
    while (1)
    {
    }
#else
    throw std::runtime_error("GenV has halted.");
#endif
}