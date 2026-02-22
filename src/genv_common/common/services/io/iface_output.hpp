/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * iface_output.hpp - Created on 22-02-2026
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

#include <cstdint>
#include "common/services/io/iostrings.hpp"
#include "common/util/hash.hpp"
#include "player.hpp"

namespace IO { class PlayerManager; }

namespace Output
{
    using namespace IO;

    enum class OutputType : uint8_t
    {
        None,
        BINARY,
        ANALOG,
        FORCE_FEEDBACK
    };

    struct IOutputDevice
    {
        friend class IO::PlayerManager;
        friend class OutputManager;

    private:
        uint8_t m_listID   = 0xFF;                 // Internal use only: Specifies the exact position of the device within the set player.
        PlayerIndex player = PlayerIndex::INVALID; // Internal use only: When registered, this is changed to be the current player the device is registered to.

    public:
        const char *name    = szNullInputDevice; // Friendly printable name
        util::Hash id       = idNull;            // Device ID - Usage is defined by the driver implementation
        util::Hash subid    = idNull;            // Device SubID - Usage is defined by the driver implementation
        OutputType type     = OutputType::None;  // Device Type - Tells the input mapper what class the device is
        uint8_t subBusID    = 0;                 // Sub Bus ID - Usage is defined by the driver implementation
        uint8_t numFeedback = 0;                 // Number of feedback/output drivers

        constexpr IOutputDevice() {}

        constexpr IOutputDevice(
            const char *name, util::Hash id, util::Hash subid,
            OutputType type, uint8_t subBusID,
            uint8_t numFeedback = 0)
        {
            this->name        = name;
            this->id          = id;
            this->subid       = subid;
            this->type        = type;
            this->subBusID    = subBusID;
            this->numFeedback = numFeedback;
        }
    };
} // namespace Output