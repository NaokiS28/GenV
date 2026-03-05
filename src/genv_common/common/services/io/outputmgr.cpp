/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * outputmgr.cpp - Created on 26-02-2026
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

#include <stddef.h>
#include <string.h>

#include "outputmgr.hpp"
#include "common/logger/log.hpp"

#define OMLOG(fmt, ...) LOG("outputmgr", fmt __VA_OPT__(, ) __VA_ARGS__)

namespace IO
{

    // -------------------------------------------------------------------------
    // Physical device registry
    // -------------------------------------------------------------------------

    void OutputManager::registerDevice(Output::IOutputDevice *device)
    {
        if (!device)
            return;

        if (m_deviceCount >= maxTrackedOutputDevices)
        {
            OMLOG("registerDevice: registry full, cannot add device.");
            return;
        }

        // Avoid double-registration
        for (uint8_t i = 0; i < m_deviceCount; i++)
            if (m_devices[i] == device) return;

        m_devices[m_deviceCount++] = device;
    }

    void OutputManager::unregisterDevice(Output::IOutputDevice *device)
    {
        if (!device)
            return;

        // Tombstone all slot map entries whose port lies within this device's bank
        tombstonePortsForDevice(device);

        // Remove from registry
        for (uint8_t i = 0; i < m_deviceCount; i++)
        {
            if (m_devices[i] == device)
            {
                // Shift remaining entries down
                for (uint8_t j = i; j < m_deviceCount - 1; j++)
                    m_devices[j] = m_devices[j + 1];
                m_devices[--m_deviceCount] = nullptr;
                return;
            }
        }

        OMLOG("unregisterDevice: device not found in registry.");
    }

    void OutputManager::tombstonePortsForDevice(const Output::IOutputDevice *device)
    {
        if (!device || !device->bank || device->numFeedback == 0)
            return;

        const Output::OutputPortBase *bankBegin = device->bank;
        const Output::OutputPortBase *bankEnd   = device->bank + device->numFeedback;

        for (auto &slotMap : m_slotMaps)
        {
            // Collect keys to remove first, then remove after iteration,
            // to avoid modifying the map while forEach is walking buckets.
            util::Hash toRemove[maxOutputMappingsPerSlot];
            uint8_t    removeCount = 0;

            slotMap.forEach([&](util::Hash key, OutputPortRef &ref) {
                const Output::OutputPortBase *p = ref.port();
                if (p >= bankBegin && p < bankEnd)
                    toRemove[removeCount++] = key;
            });

            for (uint8_t i = 0; i < removeCount; i++)
                slotMap.remove(toRemove[i]);
        }
    }

    // -------------------------------------------------------------------------
    // Logical name mapping
    // -------------------------------------------------------------------------

    bool OutputManager::mapOutput(
        uint8_t slotIndex, util::Hash logicalId, OutputPortRef ref)
    {
        if (slotIndex > 8)
        {
            OMLOG("mapOutput: invalid slot index %u.", slotIndex);
            return false;
        }
        if (!m_slotMaps[slotIndex].insert(logicalId, ref))
        {
            OMLOG("mapOutput: slot %u map is full.", slotIndex);
            return false;
        }
        return true;
    }

    OutputPortRef OutputManager::findOutput(
        uint8_t slotIndex, util::Hash logicalId) const
    {
        if (slotIndex > 8) return OutputPortRef{};

        const OutputPortRef *ref = m_slotMaps[slotIndex].find(logicalId);
        return ref ? *ref : OutputPortRef{};
    }

    OutputPortRef OutputManager::findOutput(util::Hash logicalId) const
    {
        for (uint8_t i = 0; i <= 8; i++)
        {
            const OutputPortRef *ref = m_slotMaps[i].find(logicalId);
            if (ref && ref->valid()) return *ref;
        }
        return OutputPortRef{};
    }

    void OutputManager::unmapOutput(uint8_t slotIndex, util::Hash logicalId)
    {
        if (slotIndex > 8) return;
        m_slotMaps[slotIndex].remove(logicalId);
    }

    void OutputManager::clearSlot(uint8_t slotIndex)
    {
        if (slotIndex > 8) return;
        m_slotMaps[slotIndex].clear();
    }

    void OutputManager::clearAll()
    {
        for (auto &slotMap : m_slotMaps)
            slotMap.clear();

        memset(m_devices, 0, sizeof(m_devices));
        m_deviceCount = 0;
    }

} // namespace IO
