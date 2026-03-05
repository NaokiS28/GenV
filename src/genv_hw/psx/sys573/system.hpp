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

#include "common/services/system/arcade/iface_arcade.hpp"
#include "common/services/system/system.hpp"
#include "common/services/system/arcade/arcade.hpp"

#include "psx/common/system.hpp"
#include "psx/common/drivers/video/video.hpp"

#include "io/io.hpp"
#include "rtc.hpp"

// #include "psx/common/drivers/psx_pcdrv.hpp"

namespace System573
{
    namespace KSYS573
    {
        constexpr const char *szSystemName = "System 573";
        constexpr const char *szMakeName   = "KONAMI";
    } // namespace KSYS573

    class Sys573System : public PSX::BasePSXSystem, public System::BaseArcadeSystem
    {
    private:
        System::SystemInfo si573 = {
            .type  = System::SYS_Arcade,
            .make  = KSYS573::szMakeName,
            .name  = KSYS573::szSystemName,
            .flags = System::SYS_No_Window_Mode};

        static const uint8_t system_outputBanks = 1;

        RTC::M48T58 m_rtc;
        IO::JAMMA m_jamma;
        IO::JVS m_jvs;

    public:
        Sys573System();
        ~Sys573System();

        int initCore() override;
        int initVideo() override;
        int initAudio() override;
        int initIO() override;
        int initStorage() override;

        int update() override;
        void shutdown() override;

        int readNVRAM(uint8_t *data, int offset, int count) override
        {
            return m_rtc.readNVRAM(data, offset, count);
        }
        int writeNVRAM(const uint8_t *data, int offset, int count) override
        {
            return m_rtc.writeNVRAM(data, offset, count);
        }

        System::CoinCounter increaseCoinCounter(System::CoinCounter counter) override;

        inline void tickWatchdog(void) override
        {
            if (enableWatchdogTicking) IO::TickWatchdog();
        }

        const System::SystemInfo *getSysInfo() const override
        {
            return &si573;
        }

        uint8_t setOutputs(uint8_t bank, uint8_t data) override;
        uint8_t setSingleOutput(uint8_t outputNumber, bool state) override;

        const char *getWorkingDirectory() override;
    };

} // namespace System573