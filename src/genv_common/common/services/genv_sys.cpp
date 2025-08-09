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

#include "video/nullvideo.hpp"
#include "audio/nullaudio.hpp"
#include "io/nullinput.hpp"
#include "storage/nullstorage.hpp"

#include "hardware.hpp"

void GenvSystemClass::startup()
{
    // Pointer for system and reference for app manager. Again, this seems... not right.
    // Works... but not right. Context: This stuff was moved into here but was in main.
    System::ISystem *system = System::makeNewSystem();
    Audio::IAudio *audio = new Audio::NullAudio();
    Video::IVideo *video = new Video::NullVideo();
    Input::IInput *input = new Input::NullInput();
    Files::IStorage *storage = new Files::NullStorage();
    Apps::AppManager *app = new Apps::AppManager();

    LOG_APP("Starting GenV...");    
    Services::setStorage(adminKey, storage);
    Services::setInput(adminKey, input);
    Services::setAudio(adminKey,audio);
    Services::setVideo(adminKey, video);
    Services::setSystem(adminKey, system);

    if (system == nullptr || !system->init())
    { // Always init system manager first.
        LOG_APP("System manager failed to init.");
        halt();
    }

    LOG_APP("Starting application manager...");
    Services::setAppManager(adminKey, app);
    if (app == nullptr || !app->init()){
        LOG_APP("Application manager failed to init.");
        halt();
    }
}

void GenvSystemClass::shutdown()
{
    LOG_APP("GenV is shutting down...");
    Services::destroyAppManager(adminKey);
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
    LOG_APP("GenV has halted.");
    while (1)
    {
    }
#else
    return return_code;
#endif
}