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

#include "../system.hpp"
#include "registers573.hpp"
#include "common/services/arcade/arcade.hpp"

namespace System
{
    namespace KSYS573
    {
        constexpr const char *szSystemName = "System 573";
        constexpr const char *szMakeName = "KONAMI";
    }

    class Sys573System : public PSXSystem, public ArcadeSystem
    {
    private:
        SystemInfo si573 = {
            .type = SYS_Arcade,
            .make = KSYS573::szMakeName,
            .name = KSYS573::szSystemName,
            .flags = SYS_No_Window_Mode};

        int initAudio();
        int initIO() { return 0; }
        int initFiles();

        uint32_t getJAMMAInputs(void);

        uint8_t outputBanks = 1;

    public:
        Sys573System() : PSXSystem() {}
        ~Sys573System() = default;

        bool init() override;     // Registers Windows app class and inits drivers
        int update() override;    // Process wWindows messages
        bool shutdown() override; // Prepare drivers and app for close

        int readNVRAM() override;
        int writeNVRAM() override;

        uint8_t increaseCoinCounter(uint8_t counter) override;

        void tickWatchdog(void) override
        {
            if (enableWatchdogTicking)
            {
                SYS573_WATCHDOG = 0;
            }
        }

        const SystemInfo *getSysInfo() const override
        {
            return &si573;
        }

        uint8_t setOutputs(uint8_t bank, uint8_t data) override;
        uint8_t setSingleOutput(uint8_t outputNumber, bool state) override;
    };
}