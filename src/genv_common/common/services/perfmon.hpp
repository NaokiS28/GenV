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

#include "system/system.hpp"

namespace System
{
    constexpr const unsigned int MS_1HZ = 1000;
    constexpr const unsigned int US_1HZ = 1000000;

    enum PerformanceGraphStyle : uint8_t
    {
        PERFMON_GRAPH_PIE,
        PERFMON_GRAPH_BAR,
        PERFMON_GRAPH_TIME
    };

    // All times are in microseconds for PERFMON_GRAPH_TIME, or percentage (0-100) for PIE/BAR.
    struct PerformanceGraph
    {
        size_t systemTime    = 0; // Platform/system driver update (e.g. window messages, hardware polling)
        size_t storageTime   = 0; // Block storage driver CPU processing (excludes async I/O transfers)
        size_t inputTime     = 0; // Input driver polling and state update
        size_t appTime       = 0; // Application logic update
        size_t renderTime    = 0; // Application rendering (GPU command submission)
        size_t idleTime      = 0; // Remaining CPU time unused within the frame budget
        size_t coroutineTime = 0; // Low-priority coroutine services and vsync wait
        size_t loopTime      = 0; // Accumalitve total of all functions
        size_t cycleTime     = 0; // Total frame budget based on display refresh rate

        PerformanceGraphStyle style = PERFMON_GRAPH_PIE;
    };

    class PerformanceMonitor
    {
    private:
        size_t loopStartTime       = 0;
        size_t checkpoint          = 0;
        size_t systemExecTime      = 0;
        size_t appExecTime         = 0;
        size_t inputUpdateTime     = 0;
        size_t storageUpdateTime   = 0;
        size_t renderTime          = 0;
        size_t cycleTime           = 0;
        size_t idleTime            = 0;
        size_t lastFrame           = 0;
        size_t coroutineUpdateTime = 0;

        PerformanceGraph lastGraph;
        PerformanceGraph nextGraph;

        inline size_t elapsed()
        {
            size_t now   = System::micros();
            size_t delta = now - checkpoint;
            checkpoint   = now;
            return delta;
        }

    public:
        inline void loopStart()
        {
            loopStartTime = System::micros();
            checkpoint    = loopStartTime;
        }
        inline void finishSystemExec() { systemExecTime = elapsed(); }
        inline void finishStorageUpdate() { storageUpdateTime = elapsed(); }
        inline void finishInputUpdate() { inputUpdateTime = elapsed(); }
        inline void finishAppExec() { appExecTime = elapsed(); }
        inline void finishRender() { renderTime = elapsed(); }
        inline void finishCoroutines() { coroutineUpdateTime = elapsed(); }

        PerformanceGraph &getPerformanceGraph(PerformanceGraphStyle style);
    };

    extern PerformanceMonitor PerfMon;
} // namespace System