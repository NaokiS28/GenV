/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * jvs.cpp - Created on 26-11-2025
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

/*
    System 573 JVS Driver
    =====================

    The JVS subsystem on the System 573 seems like it could be sophisticated but
    in reality the implementation is much simpler. There is a Hitatchi H8/3644
    MCU which is connected to an RS485 transceiver chip and then to the System 573
    JVS port and it handles the serial translation of the JVS bytes to and from nodes.
    About the only help it otherwise provides is automatic escaping of none-starting
    0xE0 and escape 0xD0 bytes, but otherwise the interface is entirely software driven.

    Also made perhaps more annoying is the decision to use 16-bits to transmit
    data to and from the MCU, where the first byte is the LSB of the 16-bit word
    and the second byte is the MSB. This compared to JVS's 8-Bit nature, so packets
    need to be transmitted as 16-bit chunks.
*/

#include "jvs.hpp"
#include "common/return_codes.hpp"
#include "common/services/io/iface_input.hpp"
#include "common/services/system/iface_coroutine.hpp"
#include "psx/common/system/sys.h"
#include "psx/sys573/io/jvs/h8.hpp"

namespace System573::IO
{
    using namespace H8;
    class JVS_Packet_Coroutine : public ICoroutine
    {
        JVS_Packet packet;
        uint8_t emitted = 0;

        enum JVSPacketStep
        {
            JVS_SYNC_ID,
            JVS_SIZE_DATA,
            JVS_DATA_SUM
        } packetStep = JVS_SYNC_ID;

    public:
        constexpr JVS_Packet_Coroutine(JVS_Packet packet) : ICoroutine("JVSPacket"_h)
        {
        }

        void start()
        {
        }

        bool resume()
        {
            static uint16_t word = 0;
            switch (packetStep)
            {
            case JVS_SYNC_ID:
                if (!word) word = (packet.sync | (packet.nodeID << 8));
                if (H8::writeWord(word))
                {
                    packetStep = JVS_SIZE_DATA;
                    word       = 0;
                }
                break;
            case JVS_SIZE_DATA:
                if (!word) word = (packet.size | (packet.data[0] << 8));
                if (H8::writeWord(word))
                {
                    packetStep = JVS_DATA_SUM;
                    word       = 0;
                    emitted++;
                }
                break;
            case JVS_DATA_SUM:
                if (!word) word = (packet.data[emitted] | (packet.data[emitted + 1] << 8));
                if (H8::writeWord(word))
                {
                    word = 0;
                    emitted += 2;
                }
                if (emitted >= packet.size)
                {
                    m_step = CoroutineStep::Finished;
                    return true;
                }
                break;
            }
            return false;
        }

        const char *name() { return "JVSPacket"; }
    };

    constexpr IInputDevice
    jvs(uint32_t *digital, int16_t *analog = nullptr)
    {
        return {
            GX700_JVS_NAME,
            "JVS"_h,
            0,
            Input::DEVICE_TYPE_CONTROLLER,
            0,
            0,
            digital,
            0,
            analog};
    };

    int JVS::init()
    {
        enum
        {
            H8_RESET_CHECK,
            H8_HEADER_CHECK,
            H8_INVALID_CHECK,
            DONE
        } h8_init_step = H8_RESET_CHECK;

        // Test the H8 is responding as we expect
        for (;;)
        {
            H8::reset();
            psx_delayMicrosecondsBusy(10);
            H8::ack();
            H8Status status = H8_INVALID_STATUS_CODE;
            H8Error error   = H8_INVALID_ERROR_CODE;
            switch (h8_init_step)
            {
            case H8_RESET_CHECK:
                status = H8::status();
                error  = H8::error();
                if (status != H8_STATUS_WAITING || error != H8_NO_ERROR)
                    return GV_ERROR(GV_SERVICE_INPUT, GV_CATEGORY_GENERIC, GV_ERR_NO_RESPONSE);
                break;
            case H8_HEADER_CHECK:
                H8::writeWord(0x00E0);
                psx_delayMicrosecondsBusy(10);
                status = H8::status();
                error  = H8::error();
                if (status != H8_STATUS_TRANSFER_IN_PROGRESS || error != H8_NO_ERROR)
                    return GV_ERROR(GV_SERVICE_INPUT, GV_CATEGORY_GENERIC, GV_ERR_NO_RESPONSE);
                break;
            case H8_INVALID_CHECK:
                H8::writeWord(0x001F);
                psx_delayMicrosecondsBusy(10);
                status = H8::status();
                error  = H8::error();
                if (status != H8_STATUS_BUSY || error != H8_ERROR_INVALID_SYNC)
                    return GV_ERROR(GV_SERVICE_INPUT, GV_CATEGORY_GENERIC, GV_ERR_NO_RESPONSE);
                h8_init_step = DONE;
                break;
            default: return GV_OK;
            }
        }
        return GV_OK;
    }

    bool JVS::update()
    {
        return false;
    }

    void JVS::processPackets_()
    {
        if (_packetBuffer.available())
        {
        }
    }

    uint8_t JVS::setOutputs(uint8_t bank, uint8_t data)
    {
        return 0;
    }

    uint8_t JVS::setSingleOutput(uint8_t outputNumber, bool state)
    {
        return 0;
    }

    uint8_t JVS::increaseCoinCounter(uint8_t counter)
    {
        return 0;
    }

} // namespace System573::IO
