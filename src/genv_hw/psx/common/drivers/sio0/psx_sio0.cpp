/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * SIO0_Bus.cpp - Created on 09-11-2025
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

#include <stdint.h>
#include <stddef.h>

#include "psx_sio0.hpp"
#include "common/logger/log.hpp"
#include "psx/common/registers.hpp"
#include "psx/common/system/sys.h"

// TODO: SIO0 Driver needs to be rewritten both to handle multitaps better but also to run controllers at 1MHz when in a multitap else there is *severe* slowdown.

namespace PSX
{
    class PSXSystem;
}

namespace PSX::IO
{
    static constexpr int _SIO0_BAUD_RATE = 250000;
    static constexpr int _ACK_TIMEOUT    = 120;
    static constexpr int _CS_DELAY       = 60;

    int SIO0_Bus::init()
    {
        if (_initialised)
            return 0;

        SIO_CTRL(0) = SIO_CTRL_RESET;
        SIO_MODE(0) = SIO_MODE_BAUD_DIV1 | SIO_MODE_DATA_8;
        SIO_BAUD(0) = F_CPU / _SIO0_BAUD_RATE;
        SIO_CTRL(0) = SIO_CTRL_TX_ENABLE | SIO_CTRL_RX_ENABLE | SIO_CTRL_DSR_IRQ_ENABLE;
        mouseFix();
        _initialised = true;

        // static_cast<PSX::BasePSXSystem *>(getSystem())->registerISR(SYSTEM_CALLBACK("SIO0 ISR", SIO0_Bus, m_sioISR), IRQ_SIO0);

        _initResult = 0;
        return _initResult;
    }

    // The primary function of update_() is just to make sure the official Sony PS1 mouse doesn't
    // lock up the bus. It's probably not required but none the less. Because both the Joypad and
    // memory card drivers will ping this, we have to take any expected max time out and multiply
    // by 4 since there's two instances of PSXJoy and PSXMemCard. 4 frames * 4 = 16 outta be plenty.
    void SIO0_Bus::update()
    {
        static int ackCount = 0;
        if (IRQ_STAT & (1 << IRQ_SIO0))
            ackCount++; // ACK Asserted
        else
            ackCount = 0; // ACK not asserted, we can be sure the bus is "normal"
        if (ackCount >= 16)
        {
            // ACK stuck?
            LOG("sio0", "ACK held low? Send byte to reset bus on port 1 and 2");
            mouseFix();
            ackCount = 0;
        }
    }

    // To help with PSX mouse when /ACK is stuck low
    void SIO0_Bus::mouseFix()
    {
        start(ADDR_CONTROLLER, SIO0_Port::PORT1);
        stop();
        start(ADDR_CONTROLLER, SIO0_Port::PORT2);
        stop();
    }

    uint8_t SIO0_Bus::exchangeByte(uint8_t value)
    {
        while (!(SIO_STAT(0) & SIO_STAT_TX_NOT_FULL))
            __asm__ volatile("");

        SIO_CTRL(0) |= SIO_CTRL_ACKNOWLEDGE;
        SIO_DATA(0) = value;

        while (!(SIO_STAT(0) & SIO_STAT_RX_NOT_EMPTY))
            __asm__ volatile("");

        return SIO_DATA(0);
    }

    size_t SIO0_Bus::exchangeBytes(
        const uint8_t *request,
        uint8_t *response,
        size_t reqLength,
        size_t maxRespLength,
        bool hasLastACK)
    {
        size_t respLength = 0;

        while (respLength < maxRespLength)
        {
            uint8_t byte = exchangeByte(reqLength ? *(request++) : 0);
            respLength++;

            if (reqLength)
                reqLength--;
            if (response)
                *(response++) = byte;

            // Devices will not trigger /ACK after the last response byte.
            if (hasLastACK || (respLength < maxRespLength))
            {
                if (!psx_waitForInterrupt(IRQ_SIO0, _ACK_TIMEOUT))
                    break;
            }
            psx_delayMicrosecondsBusy(_CS_DELAY);
        }

        return respLength;
    }

    int SIO0_Bus::start(uint8_t address, SIO0_Port port)
    {
        if (_inUse)
            return SIO0_IN_USE;

        SIOControlFlag cs = (port == SIO0_Port::PORT1 ? SIO_CTRL_CS_PORT_1 : SIO_CTRL_CS_PORT_2);
        SIO_CTRL(0)       = cs | SIO_CTRL_DTR | SIO_CTRL_TX_ENABLE | SIO_CTRL_RX_ENABLE | SIO_CTRL_DSR_IRQ_ENABLE | SIO_CTRL_ACKNOWLEDGE;
        psx_delayMicrosecondsBusy(_CS_DELAY);

        IRQ_STAT    = ~(1 << IRQ_SIO0);
        SIO_DATA(0) = address;

        // The controller only pulses /ACK for a brief period of time and the DSR
        // status bit in the SIO_STAT register is not latched, so the only way to
        // detect the pulse reliably is to have it trigger a dummy (latched) IRQ and
        // check for it.
        if (!psx_waitForInterrupt(IRQ_SIO0, _ACK_TIMEOUT))
            return SIO0_NO_RESPONSE;
        while (SIO_STAT(0) & SIO_STAT_RX_NOT_EMPTY)
            SIO_DATA(0);

        _inUse = true;
        return SIO0_OKAY;
    }

    void SIO0_Bus::stop(void)
    {
        psx_delayMicrosecondsBusy(_CS_DELAY);
        SIO_CTRL(0) = SIO_CTRL_TX_ENABLE | SIO_CTRL_RX_ENABLE | SIO_CTRL_DSR_IRQ_ENABLE;
        _inUse      = false;
    }

    void SIO0_Bus::m_sioISR()
    {
        return;
        if (awaiting_ack)
        {
            awaiting_ack = false;
            ack_received = true;
        }
    }

    Input::Player SIO0_Bus::psxPlayerSelect(SIO0_Port port, Multitap_Port subport)
    {
        if (port == SIO0_Port::PORT1)
        {
            switch (subport)
            {
            default:
            case Multitap_Port::PORTA: return Input::Player::PLAYER_1;
            case Multitap_Port::PORTB: return Input::Player::PLAYER_2;
            case Multitap_Port::PORTC: return Input::Player::PLAYER_3;
            case Multitap_Port::PORTD: return Input::Player::PLAYER_4;
            }
        }
        else
        {
            if (getMultitapState(SIO0_Port::PORT1) == MT_IS_PRESENT)
            {
                switch (subport)
                {
                default:
                case Multitap_Port::PORTA: return Input::Player::PLAYER_5;
                case Multitap_Port::PORTB: return Input::Player::PLAYER_6;
                case Multitap_Port::PORTC: return Input::Player::PLAYER_7;
                case Multitap_Port::PORTD: return Input::Player::PLAYER_8;
                }
            }
            else
            {
                switch (subport)
                {
                default:
                case Multitap_Port::PORTA: return Input::Player::PLAYER_2;
                case Multitap_Port::PORTB: return Input::Player::PLAYER_3;
                case Multitap_Port::PORTC: return Input::Player::PLAYER_4;
                case Multitap_Port::PORTD: return Input::Player::PLAYER_5;
                }
            }
        }
    }
} // namespace PSX::IO