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

#include "common/services/system/iface_driver.hpp"
#include "common/services/io/iface_input.hpp"
#include "common/services/system/arcade/iface_arcade.hpp"
#include "common/util/templates.hpp"
#include "psx/sys573/io/asic.hpp"

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

    class JVS : public System::IDriver
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
        System::CoinCounter increaseCoinCounter(System::CoinCounter counter);

        // Returns true if the JVS Sense input pin is being asserted.
        // JVS says the sense pin must be 2.5v when an IO board is connected but not
        // set with an ID, and 0v when ID has been given. If this function returns false,
        // an IO board is not present.
        static inline bool Sense()
        {
            return (ASIC::Regs::MiscIn & ASIC::IN_JVS_SENSE) != ASIC::IN_NONE;
        }
    };
} // namespace System573::IO