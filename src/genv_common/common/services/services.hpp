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

#include "video/video.hpp"
#include "audio.hpp"
#include "input.hpp"
#include "appmgr.hpp"
#include "storage.hpp"
#include "system.hpp"
#include "arcade/arcade.hpp"

namespace Audio
{
    class NullAudio : public IAudio
    {
    public:
        NullAudio();
        ~NullAudio();
        bool init() override;
        bool reset() override;
        void shutdown() override;
        bool play(Audio::SoundObject *sObj) override;
        bool stop(Audio::SoundObject *sObj) override;
        bool pause(Audio::SoundObject *sObj) override;
        bool isPlaying(Audio::SoundObject *sObj) override;
        int uploadSample(Audio::SoundObject *sObj) override;
    };
}

namespace Input
{
    class NullInput : public IInput
    {
    public:
        NullInput();
        ~NullInput();
        bool init() override;
        bool reset() override;
        void shutdown() override;
    };
}

namespace Files
{
    class NullStorage : public IStorage
    {
    public:
        NullStorage();
        ~NullStorage();
        bool init() override;
        bool reset() override;
        void shutdown() override;
        uint8_t getDriveList(IStorageDevice *list) override;
        const char *getWorkingDirectory() override;
    };
}

// Not sure I like this setup but it does *work*

class Services
{
public:
    // Getters
    static Audio::IAudio *getAudio(void) { return s_audio; }
    static Video::IVideo *getVideo(void) { return s_video; }
    static Input::IInput *getInput(void) { return s_input; }
    static Files::IStorage *getStorage(void) { return s_storage; }
    static System::ISystem *getSystem(void) { return s_system; }

    // AppManager is a static subsystem that orchestrates "apps" running
    static Apps::AppManager *getAppMgrInstance()
    {
        static Apps::AppManager *instance = new Apps::AppManager();
        return instance;
    }

    // Setters
    static void setSystem(System::ISystem *sys) { s_system = sys; }
    static void setVideo(Video::IVideo *video) { s_video = video; }
    static void setAudio(Audio::IAudio *audio);
    static void setInput(Input::IInput *input);
    static void setStorage(Files::IStorage *storage);

    static bool startup(void);
    static void shutdown(void);

    // Forwarder function to tick the watchdog chip on systems that use one
    static void tickWatchdog(void);     
    // Forwarder function to test the watchdog chip. 
    // ⚠️ WARNING ⚠️ THIS WILL FORCIBLY REBOOT THE SYSTEM IF THE WATCHDOG IS WORKING
    static void testWatchdog(void);
    // Forwarder function to system millis with protection     
    static size_t millis(void);         
    // Forwarder function to gpu frame counter with protection
    static size_t frames(void); 
    // Returns how many frames will pass in a given timeperiod      
    static inline size_t secondsToFrames(size_t seconds)
    {
        return msToFrames(seconds * 1000);
    } 
    // Returns how many frames will pass in a given timeperiod
    static size_t msToFrames(size_t millis);    
    // Forwarder function to system random function with protection  
    static size_t random(size_t min, size_t max); 
    // Forward function to gpu getHorizontalRes function with protection
    static uint16_t getHorizontalRes(void);
    // Forward function to gpu getVerticalRes function with protection           
    static uint16_t getVerticalRes(void);   
    // Forward function to gpu getRefreshRate function with protection           
    static uint16_t getRefreshRate(void);           

private:
    // Static pointers to service implementations
    static Audio::IAudio *s_audio;
    static Video::IVideo *s_video;
    static Input::IInput *s_input;
    static Files::IStorage *s_storage;
    static System::ISystem *s_system;
};

namespace System
{
    // Forwarder function to system millis with protection     
    inline size_t millis(void) { return Services::millis(); }
    // Forwarder function to system random function with protection  
    inline size_t random(size_t min, size_t max) { return Services::random(min, max); }
}

namespace Video
{
    // Forwarder function to gpu frame counter with protection
    inline size_t frames(void) { return Services::frames(); }
    // Returns how many frames will pass in a given timeperiod
    inline size_t msToFrames(size_t millis) { return Services::msToFrames(millis); }
    // Forward function to gpu getHorizontalRes function with protection
    inline uint16_t getHorizontalRes(void) { return Services::getHorizontalRes(); }
    // Forward function to gpu getVerticalRes function with protection           
    inline uint16_t getVerticalRes(void) { return Services::getVerticalRes(); }
    // Forward function to gpu getVerticalRes function with protection           
    inline uint16_t getRefreshRate(void) { return Services::getRefreshRate(); }
}