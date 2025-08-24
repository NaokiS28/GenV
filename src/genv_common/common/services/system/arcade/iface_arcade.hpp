/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * iface_arcade.hpp - Created on 09-08-2025
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

#include "arcade.hpp"

namespace System
{

    class IArcadeSystem
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

        uint8_t setPhysicalPlayers(uint8_t players);
        uint8_t setPhysicalCoinSlots(uint8_t slots);
        virtual uint8_t getCoinCounterBuffer(int8_t slot = -1);
        virtual uint8_t addCoin(uint8_t slot, uint8_t amount);
        virtual uint8_t addServiceCoin(uint8_t slot = 0, uint8_t amount = 1);
        virtual uint8_t increaseCoinCounter(uint8_t counter);

    public:
        IArcadeSystem() = default;
        virtual ~IArcadeSystem() = default;
        
        // Get the virtual NVRAM data for read/writing
        NVRAM &getNVRAM() { return _eeprom; }
        // Get the DIP switch at a given `bank`.
        // Returns `-1` if bank does not exist or there are no DIP switches.
        int8_t getDIPSwitch(uint8_t bank)
        {
            if (_dips.array != nullptr && bank < _dips.banks)
                return _dips.array[bank];
            return -1;
        }

        // Ticks the watchdog to prevent system from rebooting
        virtual void tickWatchdog(void) {}
        // ⚠️ WARNING ⚠️ THIS WILL IN ALMOST ALL CASES CAUSE SYSTEMS TO FORCIBLY REBOOT!
        virtual void disableWatchdogTicking(void)
        {
            enableWatchdogTicking = false;
        }

        // Reads in the NVRAM data from a physical device
        virtual int readNVRAM(uint8_t *data, int offset, int count) { return 0; }
        // Writes the current NVRAM data to the physical device
        virtual int writeNVRAM(const uint8_t *data, int offset, int size) { return 0; }

        template <typename T>
        int readNVRAM_as(T *data, int offset)
        {
            int count = sizeof(T);
            return readNVRAM(data, offset, count);
        }

        template <typename T>
        int writeNVRAM_as(T *data, int offset)
        {
            int count = sizeof(T);
            return writeNVRAM(data, offset, count);
        }

        // Returns amount of coins are available to use for a player.
        // If player given is negative, will instead return the first player
        // (if any) that has coins are available for use. If it returns negative
        // no coins are available
        int8_t coinsAvailable(int8_t player = -1);

        // Returns as many coin counters as is requested by size
        uint8_t coinsAvailable(uint8_t *array, uint8_t size);

        // True if the test switch is a sliding or rocker switch. False if a push button.
        // If false, any game test menu must have an "EXIT TO GAME" menu option.
        bool testSwitchIsLatching() { return testSwitchLatching; }

        // True if the engine should go into test mode.
        // The engine will have priority over the game and will quit the game app.
        bool runTestMode() { return enableTestMode; }

        // Sets an 8-bit output bank with data.
        // Returns the bank if succesfull, returns 0xFF if bank is invalid.
        // Note: Not all banks are 8-bits wide. If a bank has less than 8 bits,
        //       only the LSBs of the data byte up to the total outputs will be used.
        //       Banks reflect physical ports. So bank 0 might be on the motherbord,
        //       bank 1 might be the first 8 bits on an IO board, bank 2 might be the
        //       second, e.t.c.
        virtual uint8_t setOutputs(uint8_t bank, uint8_t data) = 0;

        // Sets an single output with given state.
        // Returns the output number if succesful, returns 0xFF if number is invalid.
        // Note: This function follows the same convention as setOutputs() with regards
        //       to output banks. This means that if a bank has less than 8 outputs,
        //       the unused outputs will still have an output number, they just will not
        //       do anything if written to.
        virtual uint8_t setSingleOutput(uint8_t outputNumber, bool state) = 0;

        // Writes an analogue value to an analogue output (DAC/PWM/RGB/RGBA).
        // Returns the output number if succesful, returns 0xFF if number is invalid.
        // Note: Unlike digital outputs, analogOutputs will always be assigned to an output.
        virtual uint8_t writeAnalogueOut(uint8_t analogOutput, uint8_t state) = 0;

        // Writes a 16-bit analogue value to an analogue output (DAC/PWM/RGB/RGBA).
        // Returns the output number if succesful, returns 0xFF if number is invalid.
        // Note: Unlike digital outputs, analogOutputs will always be assigned to an output.
        virtual uint8_t writeAnalogueOut16(uint8_t analogOutput, uint16_t state) = 0;

        // Writes a 32-bit analogue value to an analogue output (DAC/PWM/RGB/RGBA).
        // Returns the output number if succesful, returns 0xFF if number is invalid.
        // Note: Unlike digital outputs, analogOutputs will always be assigned to an output.
        virtual uint8_t writeAnalogueOut32(uint8_t analogOutput, uint32_t state) = 0;
    };

} // namespace System