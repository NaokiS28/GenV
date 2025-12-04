/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * sys573.hpp - Created on 01-08-2025
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

#include "common/services/system/system.hpp"
#include "psx/psx/system.hpp"
#include "psx/psx/video/video.hpp"

#include "psx/sys573/io/io.h"
#include "rtc.hpp"

#include "common/services/system/arcade/arcade.hpp"

#include "jamma/jamma.hpp"

namespace System::PSX
{
    namespace KSYS573
    {
        constexpr const char *szSystemName = "System 573";
        constexpr const char *szMakeName = "KONAMI";
    } // namespace KSYS573

    class Sys573System : public PSXSystem, public BaseArcadeSystem
    {
    private:
        SystemInfo si573 = {
            .type = SYS_Arcade,
            .make = KSYS573::szMakeName,
            .name = KSYS573::szSystemName,
            .flags = SYS_No_Window_Mode};

        uint8_t outputBanks = 1;

        KSYS573::RTC _rtc;
        Sys573Jamma _jamma;

    public:
        Sys573System();
        ~Sys573System();

        int initCore() override;
        int initVideo() override;
        int initAudio() override;
        int initIO() override;
        int initStorage() override;

        int update() override;
        bool shutdown() override;

        int readNVRAM(uint8_t *data, int offset, int count) override
        {
            return _rtc.readNVRAM(data, offset, count);
        }
        int writeNVRAM(const uint8_t *data, int offset, int count) override
        {
            return _rtc.writeNVRAM(data, offset, count);
        }

        uint8_t increaseCoinCounter(uint8_t counter) override;

        void tickWatchdog(void) override
        {
            if (enableWatchdogTicking)
            {
                sys573_watchdog_kick();
            }
        }

        const SystemInfo *getSysInfo() const override
        {
            return &si573;
        }

        uint8_t setOutputs(uint8_t bank, uint8_t data) override;
        uint8_t setSingleOutput(uint8_t outputNumber, bool state) override;

        const char *getWorkingDirectory() override;
    };

} // namespace System::PSX