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
#include "errorcodes.hpp"

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

namespace Arcade
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
}

class ArcadeSystem
{
private:
    Arcade::DIPSwitches _dips;
    NVRAM _eeprom;
    Arcade::CoinData *playerCoins = nullptr;

protected:
    uint8_t physicalPlayers = 0;
    uint8_t physicalCoinSlots = 0;

    bool enableWatchdogTicking = true; // ⚠️ WARNING ⚠️ Unless you are testWatchdog(), DONT TOUCH.
    bool enableTestMode = false;

    uint8_t setPhysicalPlayers(uint8_t players);
    uint8_t setPhysicalCoinSlots(uint8_t slots);
    virtual uint8_t addCoin(uint8_t slot, uint8_t amount);
    virtual uint8_t addServiceCoin(uint8_t slot = 0, uint8_t amount = 1);
    virtual uint8_t increaseCoinCounter(uint8_t counter);

public:
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
    virtual int readNVRAM() { return -1; }
    // Writes the current NVRAM data to the physical device
    virtual int writeNVRAM() { return -1; }

    // Returns amount of coins are available to use for a player.
    // If player given is negative, will instead return the first player
    // (if any) that has coins are available for use. If it returns negative
    // no coins are available
    int8_t coinsAvailable(int8_t player = -1);

    // Returns as many coin counters as is requested by size
    uint8_t coinsAvailable(uint8_t *array, uint8_t size);

    // True if the engine should go into test mode.
    // The engine will have priority over the game and will quit the game app.
    bool gameTestMode() { return enableTestMode; }

    // Sets an 8-bit output bank with data.
    // Returns the bank if succesfull, returns 0xFF if bank is invalid.
    // Note: Not all banks are 8-bits wide. If a bank has less than 8 bits,
    //       only the LSBs of the data byte up to the total outputs will be used.
    //       Banks reflect physical ports. So bank 0 might be on the motherbord,
    //       bank 1 might be the first 8 bits on an IO board, bank 2 might be the
    //       second, e.t.c.
    virtual uint8_t setOutputs(uint8_t bank, uint8_t data);

    // Sets an single output with given state.
    // Returns the output number if succesful, returns 0xFF if number is invalid.
    // Note: This function follows the same convention as setOutputs() with regards
    //       to output banks. This means that if a bank has less than 8 outputs,
    //       the unused outputs will still have an output number, they just will not
    //       do anything if written to. 
    virtual uint8_t setSingleOutput(uint8_t outputNumber, bool state);

    // Writes an analogue value to an analogue output (DAC/PWM/RGB/RGBA).
    // Returns the output number if succesful, returns 0xFF if number is invalid.
    // Note: Unlike digital outputs, analogOutputs will always be assigned to an output.
    virtual uint8_t writeAnalogueOut(uint8_t analogOutput, uint8_t state);

    // Writes a 16-bit analogue value to an analogue output (DAC/PWM/RGB/RGBA).
    // Returns the output number if succesful, returns 0xFF if number is invalid.
    // Note: Unlike digital outputs, analogOutputs will always be assigned to an output.
    virtual uint8_t writeAnalogueOut16(uint8_t analogOutput, uint16_t state);

    // Writes a 32-bit analogue value to an analogue output (DAC/PWM/RGB/RGBA).
    // Returns the output number if succesful, returns 0xFF if number is invalid.
    // Note: Unlike digital outputs, analogOutputs will always be assigned to an output.
    virtual uint8_t writeAnalogueOut32(uint8_t analogOutput, uint32_t state);
};