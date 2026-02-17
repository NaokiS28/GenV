/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * system.hpp - Created on 07-05-2025
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

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "common/services/services.hpp"
#include "iface_system.hpp"

#define LOG_SYS(fmt, ...) LOG("system", fmt __VA_OPT__(, ) __VA_ARGS__)

namespace System
{
    // System status messages for reporting conditions requiring the application's attention.
    enum : uint8_t
    {
        SM_NORMAL,
        SM_RELOAD,
        SM_RESIZE,
        SM_REPOSITION,
        SM_QUIT
    };

    /*
        System Interface
        ============
        This interface is the one the main program loop uses to handle the 'system', or
        the host device the code is running on. This is to abstract the platform specific
        code away from the game/application code. The main objectives of this class is to
        init the hardware/platform of the system and then manage platform specific drivers.

        The system manager interface should create and init the following drivers at the
        minimum during init():
        * Video
        * Audio
        * Input
        * File/Block storage devices

        Once created and init'd, the driver instances should be set to the global space
        using:
        * getServiceManager()->setVideo(IVideo *gpu);
        * getServiceManager()->setAudio(IAudio *audio);
        * getServiceManager()->setInput(IInputDriver *input);
    */

    enum SysType : uint32_t
    {
        SYS_Unknown,
        SYS_Arcade,
        SYS_Console,
        SYS_Computer,
        SYS_Handheld
    };

    constexpr const char *szSystemType_Console = "Console";
    constexpr const char *szSystemType_Computer = "Computer";
    constexpr const char *szSystemType_Arcade = "Arcade";
    constexpr const char *szSystemType_Handheld = "Handheld";
    constexpr const char *szSystemType_Unknown = "Unknown";

    constexpr const char *getSystemTypeString(System::SysType type)
    {
        switch (type)
        {
        case System::SYS_Console:
            return szSystemType_Console;
        case System::SYS_Computer:
            return szSystemType_Computer;
        case System::SYS_Arcade:
            return szSystemType_Arcade;
        case System::SYS_Handheld:
            return szSystemType_Handheld;
        default:
            return szSystemType_Unknown;
        }
    }

    enum SysFlags : uint32_t
    {
        SYS_No_Window_Mode = (1 << 0),
        SYS_No_Switch_Res = (1 << 1),
    };

    struct SystemInfo
    {
        SysType type = SysType::SYS_Unknown;
        const char *make = nullptr;
        const char *name = nullptr;
        const char *osname = nullptr;
        uint32_t flags = 0;
    };

    size_t millis();
    size_t micros();
    size_t random(size_t min, size_t max);
    bool getTime(tm &time);
    size_t getTime();

    class BaseSystem : public ISystem
    {
    protected:
        ServiceManager &services;
        AdminClass_Key adminKey;

    public:
        virtual ~BaseSystem() = default;
        BaseSystem();
    };

} // namespace System
