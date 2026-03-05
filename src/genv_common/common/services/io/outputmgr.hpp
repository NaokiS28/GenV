/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * outputmgr.hpp - Created on 26-02-2026
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

#include "iface_output.hpp"
#include "common/util/hash.hpp"
#include "common/util/hashmap.hpp"

namespace IO
{
    // Maximum number of logical output name mappings per player slot.
    // Increase if a game needs more named outputs per player/cabinet.
    constexpr size_t maxOutputMappingsPerSlot = 32;

    // Maximum number of output devices tracked across the whole registry.
    // 9 slots × maxOutputDevicesPerPlayer (4) = 36.
    constexpr size_t maxTrackedOutputDevices = 36;

    // -------------------------------------------------------------------------
    // OutputPortRef
    //
    // Lightweight non-owning reference to a single OutputPortBase within a
    // device's bank[]. Null-safe: if m_port is nullptr all calls are silently
    // ignored, matching the PlayerView pattern for inputs.
    // -------------------------------------------------------------------------

    class OutputPortRef
    {
        Output::OutputPortBase *m_port;

    public:
        constexpr OutputPortRef(Output::OutputPortBase *port = nullptr)
            : m_port(port) {}

        inline void on()
        {
            if (m_port) m_port->on();
        }
        inline void off()
        {
            if (m_port) m_port->off();
        }
        inline void set(int value)
        {
            if (m_port) m_port->set(value);
        }
        inline int value() const { return m_port ? m_port->value() : 0; }
        inline bool valid() const { return m_port != nullptr; }

        // Exposed for tombstone scan — OutputManager compares this against
        // a device's bank[] address range to find stale entries on detach.
        inline Output::OutputPortBase *port() const { return m_port; }
    };

    // -------------------------------------------------------------------------
    // OutputManager
    //
    // Owns the physical device registry and the per-slot logical name maps.
    //
    // Physical registry
    //   Tracks every IOutputDevice* that has been registered. On detach,
    //   unregisterDevice() walks all 9 slot maps and tombstones every
    //   OutputPortRef whose port pointer falls within that device's bank[],
    //   keeping probe chains intact for remaining entries.
    //
    // Logical name maps
    //   Nine HashMap<OutputPortRef, maxOutputMappingsPerSlot> instances:
    //   indices 0-7 = player slots, index 8 = ARCADE_CABINET.
    //   Lookup is O(1) average with no heap allocation.
    // -------------------------------------------------------------------------

    class OutputManager
    {
    public:
        // Register a device so its ports can be tombstoned on detach.
        // Called by PlayerManager::attachOutputDevice() after the device is placed.
        void registerDevice(Output::IOutputDevice *device);

        // Tombstone all port entries in all slot maps that belong to this device,
        // then remove it from the registry.
        void unregisterDevice(Output::IOutputDevice *device);

        // Insert or overwrite a logical name → port mapping for a specific slot.
        // slotIndex: 0-7 = players, 8 = ARCADE_CABINET.
        // Returns false if the slot map is full.
        bool mapOutput(uint8_t slotIndex, util::Hash logicalId, OutputPortRef ref);

        // Logical lookup scoped to one slot. Returns invalid (nullptr) OutputPortRef
        // if the name is not mapped in that slot.
        OutputPortRef findOutput(uint8_t slotIndex, util::Hash logicalId) const;

        // Global logical lookup — searches all 9 slot maps in index order (0..8).
        // Returns the first match found, or an invalid OutputPortRef.
        OutputPortRef findOutput(util::Hash logicalId) const;

        // Remove a single logical mapping from one slot (writes tombstone).
        void unmapOutput(uint8_t slotIndex, util::Hash logicalId);

        // Remove all logical mappings from one slot.
        void clearSlot(uint8_t slotIndex);

        // Remove all logical mappings and clear the device registry.
        void clearAll();

    private:
        // Per-slot logical name maps. Index 0-7 = players, 8 = ARCADE_CABINET.
        util::HashMap<OutputPortRef, maxOutputMappingsPerSlot> m_slotMaps[9];

        // Flat registry of all registered devices for tombstone-on-detach.
        Output::IOutputDevice *m_devices[maxTrackedOutputDevices] = {};
        uint8_t m_deviceCount                                     = 0;

        // Walk all slot maps and tombstone any entry whose port pointer falls
        // within [device->bank, device->bank + device->numFeedback).
        void tombstonePortsForDevice(const Output::IOutputDevice *device);
    };

    // Free function — returns the global OutputManager instance.
    // Valid after ServiceManager::createManagers().
    OutputManager *outputManager();

} // namespace IO
