/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * psxjoy.hpp - Created on 23-08-2025
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

#include <stddef.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "common/services/io/iface_input.hpp"

#include "common/util/templates.hpp"
#include "psx/common/drivers/sio0/psx_pads.hpp"
#include "psx_sio0.hpp"

namespace System::PSX
{
    class PSXSystem;
}

namespace System::PSX::IO
{
    using namespace ::Input;

    enum FeedbackType : uint8_t
    {
        FEEDBACK_NONE = 0,
        FEEDBACK_MOTOR,
        FEEDBACK_DUALMOTOR,
        FEEDBACK_WHEEL,
    };

    struct ControllerReadResponse
    {
        union
        {
            uint8_t id8[2];
            uint16_t id = 0;
        };
        uint16_t input = 0;
        struct AnalogInput
        {
            union
            {
                uint8_t x;
                uint8_t y;
                int16_t val16 = 0;
            };
        } left, right;

        ControllerReadResponse() {}
        ControllerReadResponse(const uint8_t *rsp, size_t len)
        {
            assert(len >= 4);
            id = (uint16_t)((rsp[1] << 8) | rsp[0]);
            input = (uint16_t)((rsp[3] << 8) | rsp[2]);
            if (len > 4)
            {
                left.x = rsp[6];
                left.y = rsp[7];
                right.x = rsp[4];
                right.y = rsp[5];
            }
        }
    };

    enum PSX_Joypad_Init_Sequence : uint8_t
    {
    };

    class PSX_Joypad : public IInputDriver
    {
    private:
        SIO0_Bus *m_bus;
        static uint8_t m_driverCount;

        const SIO0_Port m_portNumber;

        struct PSX_PadData
        {
            uint32_t digital = 0;
            int16_t analog[10] = {0};
            int16_t rotary[2] = {0};
            uint8_t motorStrength[2] = {0};
            JoypadType type = PAD_DISCONNECTED;
            bool doDSTest = true;
        } m_padData[4];

        IInputDevice m_padList[4];

        util::RingBuffer<uint8_t, 16> m_pad_tx;
        util::RingBuffer<uint8_t, 16> m_pad_rx;
        int m_packetSent = 0;

        int m_configMode(bool state, uint8_t subport = 0);
        int m_setAnalog(bool state = true, bool lock = true, uint8_t subport = 0);
        int m_setDualshock(bool state = true, uint8_t subport = 0);
        int m_setDS2Analog(uint32_t bitmask = 0x3FFFF, uint8_t subport = 0);

        void m_padChange(IInputDevice &pad, const ControllerReadResponse &resp, const Multitap_Port subport);
        void m_padDisconnect(IInputDevice &pad, const ControllerReadResponse &resp, const Multitap_Port subport);

        void m_processPackets();

    public:
        inline PSX_Joypad(SIO0_Port port)
            : m_portNumber(port)
        {
            assert(m_driverCount < 2);
            _name = PSX_PS_CONTROLLER_STR;
            m_bus = getSIO0_Bus();
        };

        int poll(ControllerReadResponse &resp, Multitap_Port subport);

        int init() override;
        int update() override;
        bool reset() override;
        inline void shutdown() override
        {
            reset();
        }
    }; // namespace System::PSX::IO
} // namespace System::PSX::IO