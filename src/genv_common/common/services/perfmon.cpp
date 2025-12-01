/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * perfmon.cpp - Created on 14-08-2025
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

#include "perfmon.hpp"
#include "common/util/misc.hpp"

namespace System
{
    PerformanceMonitor PerfMon;

    PerformanceGraph &PerformanceMonitor::getPerformanceGraph(PerformanceGraphStyle style)
    {
        size_t frame = Video::frames();
        if (frame == lastFrame)
        {
            return lastGraph;
        }

        lastGraph = nextGraph;
        lastFrame = frame;

        uint8_t screenRate = Video::getRefreshRate();
        cycleTime = (MS_1HZ / screenRate);

        nextGraph.style = style;
        switch (style)
        {
        case PERFMON_GRAPH_TIME:
            nextGraph.systemTime = systemExecTime;
            nextGraph.appTime = appExecTime;
            nextGraph.renderTime = renderTime;
            nextGraph.storageTime = storageUpdateTime;
            nextGraph.inputTime = inputUpdateTime;
            nextGraph.coroutineTime = coroutineUpdateTime;
            nextGraph.idleTime = cycleTime - (systemExecTime + appExecTime + renderTime + storageUpdateTime + inputUpdateTime + coroutineUpdateTime);
            nextGraph.cycleTime = cycleTime;
            break;
        default:
        case PERFMON_GRAPH_PIE:
        case PERFMON_GRAPH_BAR:
            nextGraph.systemTime = util::toPercent(systemExecTime, cycleTime);
            nextGraph.appTime = util::toPercent(appExecTime, cycleTime);
            nextGraph.renderTime = util::toPercent(renderTime, cycleTime);
            nextGraph.storageTime = util::toPercent(storageUpdateTime, cycleTime);
            nextGraph.inputTime = util::toPercent(inputUpdateTime, cycleTime);
            nextGraph.coroutineTime = util::toPercent(coroutineUpdateTime, cycleTime);
            nextGraph.idleTime = util::toPercent(cycleTime - (systemExecTime + appExecTime + renderTime + storageUpdateTime + inputUpdateTime + coroutineUpdateTime), cycleTime);
            nextGraph.cycleTime = cycleTime;
            break;
        }

        return lastGraph;
    }

} // namespace System