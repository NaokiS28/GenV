/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * jvs.hpp - Created on 26-11-2025
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

#include "common/services/io/iface_driver.hpp"
#include "common/services/io/iface_input.hpp"
#include "common/util/templates.hpp"

namespace System573::IO
{
    using namespace Input;

    constexpr const char GX700_JVS_NAME[] = "System 573 JVS";

    constexpr int maxJVSDataSize = 16;
    constexpr int maxJVSdevices  = 4;

    struct alignas(uint16_t) JVS_Packet
    {
        uint8_t sync   = 0xE0;
        uint8_t nodeID = 0x00;
        uint8_t size   = 0x00;
        uint8_t data[maxJVSDataSize]; // Contains sum
    };

    class JVS : public IDriver
    {
    private:
        void processPackets_();

        IInputDevice _devs[maxJVSdevices];
        util::RingBuffer<uint8_t, 64> _packetBuffer;

    public:
        inline JVS() { _name = GX700_JVS_NAME; }

        int init() override;
        bool update() override;

        uint8_t setOutputs(uint8_t bank, uint8_t data);
        uint8_t setSingleOutput(uint8_t outputNumber, bool state);
        uint8_t increaseCoinCounter(uint8_t counter);
    };
} // namespace System573::IO