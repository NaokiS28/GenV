/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * sys573.cpp - Created on 01-08-2025
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

#include "sys573.hpp"

#include "jamma.hpp"
#include "psx/system.hpp"

#include "common/services/services.hpp"
#include "common/services/io/iface_input.hpp"

namespace System::PSX
{
    Sys573System::Sys573System() : PSXSystem()
    {
        if (clock)
            delete clock;
        clock = &_rtc;
    }

    int Sys573System::init()
    {
        int r = 0;
        r = PSXSystem::init();

        testSwitchLatching = false; // Test switch is push button

        return r;
    }

    int Sys573System::update()
    {
        // Tick each coin counter if there's data to add.
        // Gets all counter buffers as a boolean value.
        uint8_t cc = getCoinCounterBuffer();
        for (int i = 0; cc != 0; i++)
        {
            if (cc & 0x1)
                increaseCoinCounter(i);
            cc >>= 1;
        }

        if (false)
        { // Test button read from JAMMA and JVS
        }
        return 0;
    }

    bool Sys573System::shutdown()
    {
        clock = nullptr;
        return true;
    }

    int Sys573System::_initAudio()
    {
        // IAudio *aDriver = Win32::CreateAudioDriver(Win32::AD_WIN_DSOUND,
        // gpuWnd); if (!aDriver || !aDriver->init())
        // S573 CD/DIO
        return 0;
    }

    int Sys573System::_initFiles()
    {
        // S573 CD
        int r = 0;
        mcDriver = new PSX_MemCard();
        if (!mcDriver)
            r = -2;

#ifndef NDEBUG
        pcDriver = new PSX_PCDrive();
        if (!mcDriver)
            r = -3;
#endif
        return r;
    }

    int Sys573System::_initIO()
    {
        PSXSystem::_initIO();

        Input::IInput *joyDriver = new Sys573Jamma();
        if (joyDriver)
            return -1;
        Services::addInputDevice(joyDriver);

        return 0;
    }

    uint8_t Sys573System::increaseCoinCounter(uint8_t counter)
    {
        if (counter >= physicalCoinSlots)
            return 0xFF;

        if (counter < 2)
        {
            // Shift over the counter (so it's either CC1 or CC2), set, wait, unset
            SYS573_MISC_OUT |= (SYS573_MISC_OUT_COIN_COUNT1 << counter);
            for (int i = 100; i > 0; i--)
                ;
            SYS573_MISC_OUT ^= (SYS573_MISC_OUT_COIN_COUNT1 << counter);
            return counter;
        }
        else
        {
            return increaseCoinCounter(counter);
        }
    }

    uint8_t Sys573System::setOutputs(uint8_t bank, uint8_t data)
    {
        if (bank >= outputBanks)
            return 0xff;

        if (bank == 0)
        {
            SYS573_EXT_OUT = data;
            return 0;
        }
        else
        {
            // JVS/IO
            setOutputs(bank, data);
            return 0;
        }
    }
    uint8_t Sys573System::setSingleOutput(uint8_t outputNumber, bool state)
    {
        if (outputNumber >= (8 * outputBanks))
            return 0xff;

        if (outputNumber < 8)
        {
            if (state)
                SYS573_EXT_OUT |= (uint8_t)(state << outputNumber);
            else
                SYS573_EXT_OUT &= ~(uint8_t)(state << outputNumber);
            return outputNumber;
        }
        else
        {
            // JVS/IO
            setSingleOutput(outputNumber, state);
            return outputNumber;
        }
    }

    const char *Sys573System::getWorkingDirectory()
    {
        return nullptr;
    }

    GPU::Sys573Video::Sys573Video() : PSXGPU(VRAM_2MIB)
    {
    }
} // namespace System::PSX