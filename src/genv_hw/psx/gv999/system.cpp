/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * konamigv.cpp - Created on 01-08-2025
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

#include "psx/gv999/halt/halt.h"

#include "common/services/services.hpp"
#include "psx/gv999/io/io.hpp"
#include "src/halt.h"

namespace SystemGV
{
    using namespace PS1;
    GV999System::GV999System(ServiceManager &services)
        : BasePS1System(services),
          m_jamma(*this)
    {
    }

    GV999System::~GV999System()
    {
        psx_uninstallExceptionHandler();
        if (clock)
            delete clock;
    }

    int GV999System::initCore()
    {
        BasePS1System::initCore();

        // Enable PIO/573 read/writing with delay slots. These are based on Konami's values
        // This needs to be done first else the RTC is inacessible - In theory already setup, but just in case.
        // BIU_DEV0_ADDR = 0x1F000000;
        // BIU_DEV0_CTRL = 0x24173f47;

        // RAM size should already be configured by the BIOS
        // DRAM_CTRL = 0x00000B88;

        // m_rtc.init();
        // if (clock)
        //    delete clock; // Remove the softclock
        // clock = &m_rtc;

        genv_halt_register_extension(&sysgv_extenstion);
        genv_halt_register_driver(&sysgv_watchdog_driver);

        testSwitchLatching = false; // Test switch is push button
        tickWatchdog();
        return GV_OK;
    }

    int GV999System::update()
    {
        BasePS1System::update();
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

    void GV999System::shutdown()
    {
        BasePS1System::shutdown();
        clock = nullptr;
    }

    int GV999System::initVideo()
    {
        int error = 0;
        gpu       = new GPU::PS1GPU(*this, GP1_VRAM_1MB);
        error     = ioTest(gpu, PS1_GPU_STR, PS1_CREATE_STR);
        if (!error) ioTest(gpu->init(), PS1_GPU_STR, PS1_INIT_STR);
        tickWatchdog();
        return error;
    }

    int GV999System::initAudio()
    {
        // IAudioDriver *aDriver = Win32::CreateAudioDriver(Win32::AD_WIN_DSOUND,
        // gpuWnd); if (!aDriver || !aDriver->init())
        // S573 CD/DIO
        return 0;
    }

    int GV999System::initStorage()
    {
        BasePS1System::initStorage();
        return GV_OK;
    }

    int GV999System::initIO()
    {
        BasePS1System::initIO();
        registerDriver(&m_jamma);
        return GV_OK;
    }

    System::CoinCounter GV999System::increaseCoinCounter(System::CoinCounter counter)
    {
        if (counter < System::CoinCounter::Counter2)
        {
            if (counter == System::CoinCounter::Counter1)
                IO::PulseOutput(IO::Outputs::COIN_COUNTER_1);
            else
                IO::PulseOutput(IO::Outputs::COIN_COUNTER_2);
            return counter;
        }
        return System::CoinCounter::None;
    }

    const char *GV999System::getWorkingDirectory()
    {
        return nullptr;
    }

} // namespace SystemGV