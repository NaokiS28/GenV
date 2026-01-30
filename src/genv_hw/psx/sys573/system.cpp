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

#include "system.hpp"

#include "psx/common/system.hpp"
#include "psx/common/halt/halt.h"
#include "psx/common/system/sys.h"
#include "psx/common/system/gpucmd.h"
#include "psx/common/system/pcsxhw.h"

#include "psx/sys573/halt/halt.h"

#include "common/services/services.hpp"

namespace System::PSX
{
    Sys573System::Sys573System() : BasePSXSystem()
    {
    }

    Sys573System::~Sys573System()
    {
        psx_uninstallExceptionHandler();
        if (clock)
            delete clock;
    }

    int Sys573System::initCore()
    {
        if (pcsx_present())
        {
            LOG_SYS(szRedux);
        }
        setupInterruptHandler_();
        psx_enableInterrupts();

        // Enable PIO/573 read/writing with delay slots. These are based on Konami's values
        // This needs to be done first else the RTC is inacessible - In theory already setup, but just in case.
        BIU_DEV0_ADDR = 0x1F000000;
        BIU_DEV0_CTRL = 0x24173f47;

        // RAM size should already be configured by the BIOS
        // DRAM_CTRL = 0x00000B88;

        _rtc.init();
        if (clock)
            delete clock; // Remove the softclock
        clock = &_rtc;

        testSwitchLatching = false; // Test switch is push button
        tickWatchdog();
        return GV_OK;
    }

    int Sys573System::update()
    {
        // Tick the watchdog. System updates happen after frame has rendered.
        tickWatchdog();

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

    int Sys573System::initVideo()
    {
        int error = 0;
        gpu = new GPU::PSXGPU(GP1_VRAM_2MB);
        error = ioTest(gpu, PSX_GPU_STR, PSX_CREATE_STR);
        if (!error) ioTest(gpu->init(), PSX_GPU_STR, PSX_INIT_STR);
        if (!error) services.setVideo(adminKey, gpu);
        psx_halt_append_func(sys573_halt_delay);
        tickWatchdog();
        return error;
    }

    int Sys573System::initAudio()
    {
        // IAudio *aDriver = Win32::CreateAudioDriver(Win32::AD_WIN_DSOUND,
        // gpuWnd); if (!aDriver || !aDriver->init())
        // S573 CD/DIO
        return 0;
    }

    int Sys573System::initStorage()
    {
        int port = 1;
        for (auto &mc : mcDriver)
        {
            int mcError = ioTest(mc.init(), PSX_MEMORY_CARD_STR, port, PSX_INIT_STR);
            if (!mcError) services.registerStorageDriver(&mc);
        }

#ifndef NDEBUG
        // int pcError = 0;
        // pcDriver = new Storage::PSX_PCDrive();
        // pcError = ioTest(pcDriver, PSX_PC_DRIVE_STR, PSX_CREATE_STR);
        // if (!pcError) pcError = ioTest(pcDriver->init(), PSX_PC_DRIVE_STR, PSX_INIT_STR);
        // if (!pcError) services.registerStorageDriver(pcDriver);
#endif
        return 0;
    }

    int Sys573System::initIO()
    {
        BasePSXSystem::initIO();
        services.registerInputDriver(&_jamma);

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

} // namespace System::PSX