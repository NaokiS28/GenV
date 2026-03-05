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
#include "iostrings.hpp"
#include "common/util/hash.hpp"
#include "player.hpp"

namespace IO
{
    class PlayerManager;
}

namespace Output
{
    using namespace IO;

    enum class OutputType : uint8_t
    {
        None,
        BINARY,        // Output is a dual-state lamp or LED, On or Off
        ANALOG,        // Output can be variable. Dimmable lamp or variable force motor
        FORCE_FEEDBACK // Output can have various FFB effects applied
    };

    class OutputPortBase
    {
    private:
        int currentValue = 0;
        const int offValue;
        const int minValue;
        const int maxValue;

    public:
        bool hasChanged   = false;
        util::Hash portId = idNull;

        const OutputType type;
        constexpr OutputPortBase(
            const OutputType type,
            const int offValue,
            const int minValue,
            const int maxValue)
            : offValue(offValue),
              minValue(minValue),
              maxValue(maxValue),
              type(type) {}

        constexpr OutputPortBase(
            const OutputType type,
            const int minValue,
            const int maxValue)
            : offValue(minValue),
              minValue(minValue),
              maxValue(maxValue),
              type(type) {}

        inline int value() const { return currentValue; }
        inline virtual void on() { currentValue = maxValue; }
        inline virtual void off() { currentValue = offValue; }
        inline virtual void set(int value)
        {
            int v        = value < minValue ? minValue : value;
            v            = value > maxValue ? maxValue : value;
            currentValue = v;
        }
    };

    class BinaryOutput : public OutputPortBase
    {
    public:
        constexpr BinaryOutput()
            : OutputPortBase(
                  OutputType::BINARY,
                  false,
                  true) {}
    };

    class AnalogOutput : public OutputPortBase
    {
    public:
        constexpr AnalogOutput(
            const int minValue,
            const int maxValue)
            : OutputPortBase(
                  OutputType::ANALOG,
                  minValue,
                  maxValue) {}
    };

    class FFBOutput : public OutputPortBase
    {
    public:
        constexpr FFBOutput(
            const int offValue,
            const int minValue,
            const int maxValue)
            : OutputPortBase(
                  OutputType::FORCE_FEEDBACK,
                  offValue,
                  minValue,
                  maxValue) {}

        constexpr FFBOutput(
            const int minValue,
            const int maxValue)
            : OutputPortBase(
                  OutputType::FORCE_FEEDBACK,
                  minValue,
                  maxValue) {}

        void effect(int type, int strength) {} // todo.
        void stop() {}                         // todo.
    };

    struct IOutputDevice
    {
        friend class IO::PlayerManager;
        friend class OutputManager;

    private:
        uint8_t m_listID   = 0xFF;                 // Internal use only: Specifies the exact position of the device within the set player.
        PlayerIndex player = PlayerIndex::INVALID; // Internal use only: When registered, this is changed to be the current player the device is registered to.

    public:
        const char *name     = szNullInputDevice; // Friendly printable name
        util::Hash id        = idNull;            // Device ID - Usage is defined by the driver implementation
        util::Hash subid     = idNull;            // Device SubID - Usage is defined by the driver implementation
        OutputType type      = OutputType::None;  // Device Type - Tells the input mapper what class the device is
        uint8_t subBusID     = 0;                 // Sub Bus ID - Usage is defined by the driver implementation
        uint8_t numFeedback  = 0;                 // Number of feedback/output drivers
        OutputPortBase *bank = nullptr;

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