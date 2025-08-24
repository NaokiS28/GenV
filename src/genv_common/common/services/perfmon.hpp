/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * perfmon.hpp - Created on 14-08-2025
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
#include <stddef.h>

#include "system/iface_system.hpp"

namespace System
{
    constexpr const unsigned int MS_1HZ = 1000;

    enum PerformanceGraphStyle : uint8_t
    {
        PERFMON_GRAPH_PIE,
        PERFMON_GRAPH_BAR,
        PERFMON_GRAPH_TIME
    };

    struct PerformanceGraph
    {
        size_t systemTime = 0;
        size_t storageTime = 0;
        size_t inputTime = 0;
        size_t appTime = 0;
        size_t renderTime = 0;
        size_t idleTime = 0;
        size_t cycleTime = 0;

        PerformanceGraphStyle style = PERFMON_GRAPH_PIE;
    };

    class PerformanceMonitor
    {
    private:
        size_t loopStartTime = 0;
        size_t systemExecTime = 0;
        size_t appExecTime = 0;
        size_t inputUpdateTime = 0;
        size_t storageUpdateTime = 0;
        size_t renderTime = 0;
        size_t cycleTime = 0;
        size_t lastFrame = 0;

        PerformanceGraph lastGraph;
        PerformanceGraph nextGraph;

    public:
        inline void finishSystemExec() { systemExecTime = (System::millis() - loopStartTime); }
        inline void finishAppExec() { appExecTime = (System::millis() - loopStartTime); }
        inline void finishRender() { renderTime = (System::millis() - loopStartTime); }
        inline void finishStorageUpdate() { storageUpdateTime = (System::millis() - loopStartTime); }
        inline void finishInputUpdate() { inputUpdateTime = (System::millis() - loopStartTime); }
        inline void loopStart() { loopStartTime = System::millis(); }

        PerformanceGraph &getPerformanceGraph(PerformanceGraphStyle style);
    };

    extern PerformanceMonitor PerfMon;
} // namespace System