/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * arcade.hpp - Created on 01-08-2025
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
#include "nvram.hpp"
#include "hardware.hpp" // IWYU pragma: export
#include "common/services/system/arcade/iface_arcade.hpp"

/*
 * GenV - Arcade Extenstions
 * Systems which are arcade platforms (or can work as arcade platforms natively) should
 * be derrived from this class which allows arcade game titles to access the physical
 * devices of a system.
 *
 * It is expected that the minimum of the following features are supported:
 * * Coin Inputs - Must support a minimum of 1 coin/credit device
 * * Coin Counter - Must support a minimum of 1 counter PER coin/credit device
 * * Player Inputs - This must be a minimum of 1 player with 2 directions and 3 buttons.
 * * Service Inputs - Must support a minimum of a Test button/switch and a Service credit input
 * * NVRAM Device support - This can be an battery backed RTC device, flash or other non-volatile data storage device.
 * * Watchdog - If the hardware has a watchdog, it MUST be supported by Genv.
 */

namespace System
{
    namespace ArcadeEnv
    {
        struct DIPSwitches
        {
            uint8_t banks = 0;
            uint8_t *array = nullptr;
        };

        struct CoinData
        {
            uint8_t coinsIn = 0;
            uint8_t counterTicks = 0;
        };
    } // namespace ArcadeEnv

    class BaseArcadeSystem : public IArcadeSystem
    {
    private:
        ArcadeEnv::DIPSwitches _dips;
        NVRAM _eeprom;
        ArcadeEnv::CoinData *playerCoins = nullptr;

    protected:
        uint8_t physicalPlayers = 0;
        uint8_t physicalCoinSlots = 0;

        bool enableWatchdogTicking = true; // ⚠️ WARNING ⚠️ Unless you are testWatchdog(), DONT TOUCH.
        bool testSwitchLatching = false;   // If false, enableTestMode must be disabled by the Test Menu exiting. If true, test screen will exit when enableTestMode goes low,
        bool enableTestMode = false;       // On systems with a switch, this will mirror the switch ON-OFF state. On push button systems, this will toggle on

        uint8_t setPhysicalPlayers(uint8_t players) override;
        uint8_t setPhysicalCoinSlots(uint8_t slots) override;
        virtual uint8_t getCoinCounterBuffer(int8_t slot = -1) override;
        virtual uint8_t addCoin(uint8_t slot, uint8_t amount) override;
        virtual uint8_t addServiceCoin(uint8_t slot = 0, uint8_t amount = 1) override;
        virtual uint8_t increaseCoinCounter(uint8_t counter) override;

    public:
        // Get the virtual NVRAM data for read/writing
        NVRAM &getNVRAM() override { return _eeprom; }
        // Get the DIP switch at a given `bank`.
        // Returns `-1` if bank does not exist or there are no DIP switches.
        int8_t getDIPSwitch(uint8_t bank) override
        {
            if (_dips.array != nullptr && bank < _dips.banks)
                return _dips.array[bank];
            return -1;
        }

        // Ticks the watchdog to prevent system from rebooting
        virtual void tickWatchdog(void) override {}

        // ⚠️ WARNING ⚠️ THIS WILL IN ALMOST ALL CASES CAUSE SYSTEMS TO FORCIBLY REBOOT!
        virtual void disableWatchdogTicking(void) override
        {
            enableWatchdogTicking = false;
        }

        // Reads in the NVRAM data from a physical device (if available)
        virtual int readNVRAM(uint8_t *data, int offset, int count) override { return 0; }
        // Writes the current NVRAM data to the physical device (if available)
        virtual int writeNVRAM(const uint8_t *data, int offset, int size) override { return 0; }

        int8_t coinsAvailable(int8_t player = -1) override;

        // Returns as many coin counters as is requested by size
        uint8_t coinsAvailable(uint8_t *array, uint8_t size) override;

        bool testSwitchIsLatching() override { return testSwitchLatching; }
        bool runTestMode() override { return enableTestMode; }

        uint8_t setOutputs(uint8_t bank, uint8_t data) override;
        uint8_t setSingleOutput(uint8_t outputNumber, bool state) override;

        virtual uint8_t writeAnalogueOut(uint8_t analogOutput, uint8_t state) override;
        virtual uint8_t writeAnalogueOut16(uint8_t analogOutput, uint16_t state) override;
        virtual uint8_t writeAnalogueOut32(uint8_t analogOutput, uint32_t state) override;
    };
} // namespace System

// TODO: Is this macro of any real benefit now? GetArcadeInterface does the important thing.
// This macro is a short hand to mean that this code should only be run if the system
// is an arcade system. Otherwise it is skipped. Use Genv_Arcade to access arcade
// system specific functions. Uses static_cast to avoid RTTI
#define ArcadeFunc(action)                                                 \
    do                                                                     \
    {                                                                      \
        System::IArcadeSystem *Genv_Arcade = System::getArcadeInterface(); \
        if (Genv_Arcade)                                                   \
        {                                                                  \
            action;                                                        \
        }                                                                  \
    } while (0)

#define ArcadeWatchdogKick()                                               \
    do                                                                     \
    {                                                                      \
        System::IArcadeSystem *Genv_Arcade = System::getArcadeInterface(); \
        if (Genv_Arcade)                                                   \
        {                                                                  \
            Genv_Arcade->tickWatchdog();                                   \
        }                                                                  \
    } while (0)
