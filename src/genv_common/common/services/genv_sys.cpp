/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * genv_sys.cpp - Created on 09-08-2025
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

#include "genv_sys.hpp"

#include "common/services/services.hpp"
#include "hardware.hpp"
#include "system/system.hpp"
#include "system/arcade/arcade.hpp"
#include "common/logger/log.hpp"
#include "terminal/terminal.h"

#include "video/nullvideo.hpp"
#include "audio/nullaudio.hpp"

#include "assert.h"

#ifndef GENV_COMPUTER
#include "halt_screen/halt_screen.h"
#else
#include <stdexcept>
#endif

#define GENV_LOG(fmt, ...) LOG("genv", fmt, __VA_ARGS__)

#define abortIf(func, name)                     \
    {                                           \
        int r = func;                           \
        if (r != GV_OK)                         \
        {                                       \
            GENV_LOG(failedToInitFmt, name, r); \
            halt();                             \
        }                                       \
    }

#define warnIf(func, name)                      \
    {                                           \
        int r = func;                           \
        if (r != GV_OK)                         \
        {                                       \
            GENV_LOG(failedToInitFmt, name, r); \
        }                                       \
    }

constexpr const char failedToInitFmt[] = "%s failed to init with error: %X";

void GenvSystemClass::startup()
{
    auto services = getServiceManager();

    assert(services != nullptr);

    System::ISystem *system = System::makeNewSystem();
    Audio::IAudio *audio    = new Audio::NullAudio();
    Video::IVideo *video    = new Video::NullVideo();

    assert(system != nullptr);

    services->setAudio(adminKey, audio);
    services->setVideo(adminKey, video);
    services->setSystem(adminKey, system);

    // TODO: Allow setting custom startup baud
    genv_tty_init(115200);
    GENV_LOG("GenV (" GENV_VERSION ") - Build: " GENV_BUILD);
    GENV_LOG("Genv build date: " GENV_BDATE);
    GENV_LOG("System is: %s %s", system->getSysInfo()->make, system->getSysInfo()->name);
    GENV_LOG("System type: %s", System::getSystemTypeString(system->getSysInfo()->type));

    // What constitues an 'OS' is not to be decided by GenV, but if there's a kernel or OS version number,
    // it should be shown.
    if (system->getSysInfo()->osname != nullptr)
        GENV_LOG("OS Version: %s", system->getSysInfo()->osname);

    // The starting sequence needs to be handled carefully:
    // * The system core must be set up first for handling system interrupts and similar
    // * Then the managers must be created for IO, store and such
    // * The video system is init'd first since if it is not, then the next step will fail
    // * The managers are init'd which uploads items like the default texture and default font
    abortIf(system->initCore(), "System core");
    abortIf(services->createManagers(), "Service managers");
    abortIf(system->initVideo(), "Video driver");
    abortIf(services->init(), "Init service managers");
    ArcadeFunc(Genv_Arcade->tickWatchdog());

    // Past this point, the most critical systems are running
    warnIf(system->initAudio(), "System audio driver");
    warnIf(system->initIO(), "System IO drivers");
    warnIf(system->initStorage(), "System storage driver");

    ArcadeFunc(Genv_Arcade->tickWatchdog());
}

void GenvSystemClass::shutdown()
{
    GENV_LOG("GenV system is shutting down...");
    getServiceManager()->shutdown();
}

void GenvSystemClass::halt(int return_code)
{
    shutdown();
#ifndef GENV_COMPUTER
    GENV_LOG("GenV has halted.");
    genv_halt_screen_show("GenV has halted.");
    while (1) {}
#else
    throw std::runtime_error("GenV has halted.");
#endif
}