/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * main.cpp - Created on 21-04-2025
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

#include "app/appmgr.hpp"
#include "common/services/services.hpp"
#include "common/services/genv_sys.hpp"
#include "common/services/perfmon.hpp"
#include "common/services/video/color.hpp"
#include <stdio.h>

// #define PERFMON_US
// #define PERFMON_PERCENT

GenvSystemClass genv;

int main(int argc, char *argv[])
{
    genv.startup(); // This creates and inits the GenV engine session, core services etc.

    ServiceManager *serviceManager = getServiceManager();
    Video::IVideo *video           = serviceManager->getVideo(); // It is assumed the System class will have init'd the I/O driver.
    Apps::AppManager *apps         = Apps::getAppManager();      // App manager lifecycle owned by main

    if (!apps) genv.halt();

    // char str[128]    = {0};
    uint8_t sm_state = System::SM_NORMAL;
    while (sm_state != System::SM_QUIT)
    {

#ifndef NDEBUG
#ifdef PERFMON_PERCENT
        System::PerformanceGraph perfGraph = System::PerfMon.getPerformanceGraph(System::PERFMON_GRAPH_BAR);
        snprintf(str, 128, "Loop: %d%%\r\nSys: %d%%\r\nBlock: %d%%\r\nInput: %d%%\r\nApp: %d%%\r\nRender: %d%%",
                 perfGraph.loopTime,
                 perfGraph.systemTime,
                 perfGraph.storageTime,
                 perfGraph.inputTime,
                 perfGraph.appTime,
                 perfGraph.renderTime);
#elif defined(PERFMON_US)
        System::PerformanceGraph perfGraph = System::PerfMon.getPerformanceGraph(System::PERFMON_GRAPH_TIME);
        snprintf(str, 128, "Loop: %d\r\nSys: %d\r\nBlock: %d\r\nInput: %d\r\nApp: %d\r\nRender: %d",
                 perfGraph.loopTime,
                 perfGraph.systemTime,
                 perfGraph.storageTime,
                 perfGraph.inputTime,
                 perfGraph.appTime,
                 perfGraph.renderTime);
#endif
#endif

        System::PerfMon.loopStart();
        sm_state = serviceManager->update(); // System, IO, Storage

        if (sm_state == System::SM_RESIZE) { apps->reload(); } // For windowed/computer platforms and the window has changed size.

        apps->update();
        System::PerfMon.finishAppExec();

        if (video->beginRender())
        {
            apps->render();
            System::PerfMon.finishRender();
            // video->drawRect(0, 0, 80, 70, Video::Colors::DarkGray);
            // video->drawText(str, 5, 5, (10 * 64), 10);
            video->endRender();
        }

        if (serviceManager->updateCoroutines())
        { // These are run in a lower priority to vsync. If vsync happens, the update cycle pauses
            System::PerfMon.finishCoroutines();
            video->doWaitForVSync(); // If Service manager runs out of services to update, then we pause for next cycle
        }
    }

    apps->shutdown();
    delete apps;
    genv.shutdown(); // Shuts down the driver services and deletes them

    // Past this point, none of the standard GenV functions are expected to work correctly as the system class is gone.
    return 0;
}