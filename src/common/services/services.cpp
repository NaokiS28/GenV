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

    s_audio = new Audio::NullAudio();
    s_input = new Input::NullInput();
    s_storage = new Files::NullStorage();

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

void Services::setAudio(Audio::IAudio *audio){
    if(!audio)
        return;

    if(s_audio){
        s_audio->shutdown();
        delete s_audio;
    }

    s_audio = audio;
}

void Services::setInput(Input::IInput *input){
    if(!input)
        return;

    if(s_input){
        s_input->shutdown();
        delete s_input;
    }

    s_input = input;
}

void Services::setStorage(Files::IStorage *storage){
    if(!storage)
        return;

    if(s_storage){
        s_storage->shutdown();
        delete s_storage;
    }

    s_storage = storage;
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

size_t Services::random(size_t min, size_t max){
    System::ISystem *sys = Services::getSystem();
    if(sys != nullptr) return sys->random(min, max);
    return 0;
}

size_t Services::frames(){
    Video::IVideo *gpu = Services::getVideo();
    if(gpu != nullptr) return gpu->getFrameCount();
    return 0;
}

uint16_t Services::getHorizontalRes(){
    Video::IVideo *gpu = Services::getVideo();
    if(gpu != nullptr) return gpu->getHorizontalRes();
    return 0;
}

uint16_t Services::getVerticalRes(){
    Video::IVideo *gpu = Services::getVideo();
    if(gpu != nullptr) return gpu->getVerticalRes();
    return 0;
}

size_t Services::msToFrames(size_t millis){
    Video::IVideo *gpu = Services::getVideo();
    size_t framerate = gpu->getRefreshRate();

    if(millis < 17)
        millis = 17;
    if(framerate < 20 || framerate > 480)
        framerate = 60;
    
    size_t msPerFrame = (1000 / framerate);
    return (millis / msPerFrame);
}


namespace Audio {
        NullAudio::NullAudio(){}
        NullAudio::~NullAudio(){}
        bool NullAudio::init(){ return true; }
        bool NullAudio::reset(){ return true; }
        void NullAudio::shutdown(){ return; }
        bool NullAudio::play(Audio::SoundObject *sObj){ return false; }
        bool NullAudio::stop(Audio::SoundObject *sObj){ return false; }
        bool NullAudio::pause(Audio::SoundObject *sObj){ return false; }
        bool NullAudio::isPlaying(Audio::SoundObject *sObj){ return false; }

        int NullAudio::uploadSample(Audio::SoundObject *sObj){ return Audio::IA_ERROR_INVALIDFUNC; }
}

namespace Input {
        NullInput::NullInput(){}
        NullInput::~NullInput(){}
        bool NullInput::init(){ return true; }
        bool NullInput::reset(){ return true; }
        void NullInput::shutdown(){ return; }
}

namespace Files {
        NullStorage::NullStorage(){}
        NullStorage::~NullStorage(){}
        bool NullStorage::init(){ return true; }
        bool NullStorage::reset(){ return true; }
        void NullStorage::shutdown(){ return; }
        uint8_t NullStorage::getDriveList(IStorageDevice *list) { return 0; }
        const char *NullStorage::getWorkingDirectory() { return nullptr; }
}