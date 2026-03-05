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

#include <stdint.h>

#include "common/services/io/player.hpp"
#include "common/util/enum_defs.hpp"
#include "nvram.hpp"

namespace System
{
    enum class CoinSlot
    {
        None  = 0,
        Coin1 = 1 << 0,
        Coin2 = 1 << 1,
        Coin3 = 1 << 2,
        Coin4 = 1 << 3,
    };

    ENABLE_BITWISE_OPS(CoinSlot);

    constexpr int coinSlotToIndex(CoinSlot coin)
    {
        switch (coin)
        {
        default: return -1;
        case CoinSlot::Coin1: return 0;
        case CoinSlot::Coin2: return 1;
        case CoinSlot::Coin3: return 2;
        case CoinSlot::Coin4: return 3;
        }
    }

    enum class CoinCounter
    {
        None     = 0,
        Counter1 = 1 << 0,
        Counter2 = 1 << 1,
        Counter3 = 1 << 2,
        Counter4 = 1 << 3,
    };

    enum class CoinCounterIndex
    {
        Counter1 = 0,
        Counter2 = 1,
        Counter3 = 2,
        Counter4 = 3,
        Invalid  = 0xFFFF,
    };

    ENABLE_BITWISE_OPS(CoinCounter);

    constexpr CoinCounter coinCounterIndexToFlag(uint8_t idx)
    {
        constexpr CoinCounter flags[] = {
            CoinCounter::Counter1,
            CoinCounter::Counter2,
            CoinCounter::Counter3,
            CoinCounter::Counter4};
        return (idx < 9) ? flags[idx] : CoinCounter::None;
    }

    constexpr CoinCounterIndex coinCounterToIndex(CoinCounter counter)
    {
        switch (counter)
        {
        case CoinCounter::Counter1: return CoinCounterIndex::Counter1;
        case CoinCounter::Counter2: return CoinCounterIndex::Counter2;
        case CoinCounter::Counter3: return CoinCounterIndex::Counter3;
        case CoinCounter::Counter4: return CoinCounterIndex::Counter4;
        default: return CoinCounterIndex::Invalid;
        }
    }

    class IArcadeSystem
    {
    protected:
        virtual CoinSlot setPhysicalCoinSlots(CoinSlot slots)                                = 0;
        virtual CoinSlot addCoin(CoinSlot slot, uint8_t amount)                              = 0;
        virtual CoinSlot addServiceCoin(CoinSlot slot = CoinSlot::Coin1, uint8_t amount = 1) = 0;
        virtual CoinCounter increaseCoinCounter(CoinCounter counter)                         = 0;
        virtual CoinCounter getCoinCounterBuffer(CoinSlot slot = CoinSlot::Coin1)            = 0;

    public:
        virtual ~IArcadeSystem() = default;

        // Get the virtual NVRAM data for read/writing
        virtual NVRAM &getNVRAM(void) = 0;

        // Get the DIP switch at a given `bank`.
        // Returns `-1` if bank does not exist or there are no DIP switches.
        virtual int8_t getDIPSwitch(uint8_t bank) = 0;

        // Ticks the watchdog to prevent system from rebooting
        virtual void tickWatchdog(void) = 0;

        // ⚠️ WARNING ⚠️ THIS WILL IN ALMOST ALL CASES CAUSE SYSTEMS TO FORCIBLY REBOOT!
        virtual void disableWatchdogTicking(void) = 0;

        // Reads in the NVRAM data from a physical device (if available)
        virtual int readNVRAM(uint8_t *data, int offset, int count) = 0;

        // Writes the current NVRAM data to the physical device (if available)
        virtual int writeNVRAM(const uint8_t *data, int offset, int size) = 0;

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
        virtual int8_t coinsAvailable(IO::Player player) = 0;

        // Returns as many coin counters as is requested by size
        virtual IO::Player coinsAvailable(uint8_t *array, IO::Player players) = 0;

        // True if the test switch is a sliding or rocker switch. False if a push button.
        // If false, any game test menu must have an "EXIT TO GAME" menu option.
        virtual bool testSwitchIsLatching() = 0;

        // True if the engine should go into test mode.
        // The engine will have priority over the game and will quit the game app.
        virtual bool runTestMode() = 0;

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