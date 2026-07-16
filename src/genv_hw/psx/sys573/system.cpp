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

#include "common/return_codes.hpp"
#include "common/services/system/arcade/iface_arcade.hpp"
#include "psx/common/system.hpp"
#include "psx/common/halt/src/halt.h"
#include "psx/common/system/sys.h"
#include "psx/common/drivers/video/gpucmd.hpp"

#include "psx/sys573/io/asic.hpp"
#include "psx/sys573/halt/halt.h"

#include "common/services/services.hpp"
#include "psx/sys573/io/io.hpp"
#include "psx/sys573/registers573.hpp"
#include "src/halt.h"

namespace System573
{
    using namespace PSX;
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
        BasePSXSystem::initCore();

        // Enable PIO/573 read/writing with delay slots. These are based on Konami's values
        // This needs to be done first else the RTC is inacessible - In theory already setup, but just in case.
        BIU_DEV0_ADDR = 0x1F000000;
        BIU_DEV0_CTRL = 0x24173f47;

        // RAM size should already be configured by the BIOS
        // DRAM_CTRL = 0x00000B88;

        m_rtc.init();
        if (clock)
            delete clock; // Remove the softclock
        clock = &m_rtc;

        genv_halt_register_extension(&sys573_extenstion);
        genv_halt_register_driver(&sys573_watchdog_driver);

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
        uint8_t cc = 0; // getCoinCounterBuffer();
        for (int i = 0; cc != 0; i++)
        {
            // if (cc & 0x1)
            //  increaseCoinCounter();
            cc >>= 1;
        }

        if (false)
        { // Test button read from JAMMA and JVS
        }
        return 0;
    }

    void Sys573System::shutdown()
    {
        clock = nullptr;
    }

    int Sys573System::initVideo()
    {
        int error = 0;
        gpu       = new GPU::PSXGPU(GP1_VRAM_1MB);
        error     = ioTest(gpu, PSX_GPU_STR, PSX_CREATE_STR);
        if (!error) ioTest(gpu->init(), PSX_GPU_STR, PSX_INIT_STR);
        if (!error) services.setVideo(adminKey, gpu);
        tickWatchdog();
        return error;
    }

    int Sys573System::initAudio()
    {
        // IAudioDriver *aDriver = Win32::CreateAudioDriver(Win32::AD_WIN_DSOUND,
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
            if (!mcError) registerDriver(&mc);
        }

#ifndef NDEBUG
        // int pcError = 0;
        // pcDriver = new Storage::PSX_PCDrive();
        // pcError = ioTest(pcDriver, PSX_PC_DRIVE_STR, PSX_CREATE_STR);
        // if (!pcError) pcError = ioTest(pcDriver->init(), PSX_PC_DRIVE_STR, PSX_INIT_STR);
        // if (!pcError) services.registerStorageDriver(pcDriver);
#endif
        return GV_OK;
    }

    int Sys573System::initIO()
    {
        BasePSXSystem::initIO();
        registerDriver(&m_jamma);
        registerDriver(&m_jvs);
        return GV_OK;
    }

    System::CoinCounter Sys573System::increaseCoinCounter(System::CoinCounter counter)
    {
        if (counter < System::CoinCounter::Counter2)
        {
            if (counter == System::CoinCounter::Counter1)
                IO::ASIC::PulseOutput(IO::ASIC::OUT_COIN_COUNT1);
            else
                IO::ASIC::PulseOutput(IO::ASIC::OUT_COIN_COUNT2);
            return counter;
        }
        else
            return m_jvs.increaseCoinCounter(counter);
    }

    uint8_t Sys573System::setOutputs(uint8_t bank, uint8_t data)
    {
        if (bank == 0)
        {
            IO::EXTOUT::SetState(data);
            return 0;
        }
        else
            return m_jvs.setOutputs(bank, data);
    }

    uint8_t Sys573System::setSingleOutput(uint8_t outputNumber, bool state)
    {
        if (outputNumber < 8)
        {
            if (state)
                IO::ASIC::Regs::ExtOut |= (uint8_t)(state << outputNumber);
            else
                IO::ASIC::Regs::ExtOut &= ~(uint8_t)(state << outputNumber);
            return outputNumber;
        }
        else
            return m_jvs.setSingleOutput(outputNumber, state);
    }

    const char *Sys573System::getWorkingDirectory()
    {
        return nullptr;
    }

} // namespace System573