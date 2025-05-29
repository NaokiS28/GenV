/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * services.cpp - Created on 09-05-2025
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

#include "services.hpp"
#include "common/util/log.hpp"

#include "hw/hardware.hpp"

// Static member definitions
Audio::IAudio *Services::s_audio = nullptr;
Video::IVideo *Services::s_video = nullptr;
Input::IInput *Services::s_input = nullptr;
Files::IStorage *Services::s_storage = nullptr;
System::ISystem *Services::s_system = nullptr;

bool Services::startup()
{
    // Pointer for system and reference for app manager. Again, this seems... not right.
    // Works... but not right. Context: This stuff was moved into here but was in main.
    
    s_system = new System::SYSTEM_CLASS(); // Hardware specific, see hw/hardware.hpp
    if (s_system == nullptr || !s_system->init())
    { // Always init system manager first.
        LOG_APP("System manager failed to init.");
        return false;
    }

    Apps::AppManager &apps = Services::getAppMgr();
    if (apps.init())
    {
        LOG_APP("App manager failed to init.");
        shutdown();
        return false;
    }

    return true;
}

void Services::shutdown()
{
    // Is it weird to force the system driver to start up the services but we shut them down?
    // I feel its weird
    Services::getAppMgr().shutdown();

    if (Services::s_audio)
    {
        Services::s_audio->shutdown();
        delete s_audio;
    }
    if (Services::s_video)
    {
        Services::s_video->shutdown();
        delete s_video;
    }
    if (Services::s_input)
    {
        Services::s_input->shutdown();
        delete s_input;
    }
    if (Services::s_system)
    {
        Services::s_system->shutdown();
        delete s_system;
    }
}

size_t Services::millis(){
    System::ISystem *sys = Services::getSystem();
    if(sys != nullptr) return sys->millis();
    return 0;
}