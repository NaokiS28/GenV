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
#include "io/jamma/jamma.hpp"

namespace SystemGV
{
    namespace GV999
    {
        constexpr const char *szSystemName = "GV999";
        constexpr const char *szMakeName   = "KONAMI";
    } // namespace GV999

    class GV999System : public PS1::BasePS1System, public System::BaseArcadeSystem
    {
    private:
        System::SystemInfo siGV = {
            .type  = System::SYS_Arcade,
            .make  = GV999::szMakeName,
            .name  = GV999::szSystemName,
            .flags = System::SYS_No_Window_Mode};

        static const uint8_t system_outputBanks = 1;

        // NVRAM::93C46 m_nvram;
        IO::JAMMA m_jamma;

    public:
        GV999System(ServiceManager &services);
        ~GV999System();

        int initCore() override;
        int initVideo() override;
        int initAudio() override;
        int initIO() override;
        int initStorage() override;

        int update() override;
        void shutdown() override;

        int readNVRAM(uint8_t *data, int offset, int count) override
        {
            // return m_rtc.readNVRAM(data, offset, count);
            return 1;
        }
        int writeNVRAM(const uint8_t *data, int offset, int count) override
        {
            // return m_rtc.writeNVRAM(data, offset, count);
            return 1;
        }

        System::CoinCounter increaseCoinCounter(System::CoinCounter counter) override;

        inline void tickWatchdog(void) override
        {
            if (enableWatchdogTicking) IO::TickWatchdog();
        }

        const System::SystemInfo *getSysInfo() const override
        {
            return &siGV;
        }

        const char *getWorkingDirectory() override;
    };

} // namespace SystemGV