/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * psx_sio0.hpp - Created on 09-11-2025
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
#include <stddef.h>

#include "common/services/io/player.hpp"
#include "common/services/system/iface_driver.hpp"

#define BUS_START(bus, addr, port)                      \
    {                                                   \
        int r = 0;                                      \
        if (r = bus->start(addr, port), r != SIO0_OKAY) \
        {                                               \
            bus->stop();                                \
            return r;                                   \
        }                                               \
    }

#define BUS_END(bus, data, rspLen, prefix) \
    {                                      \
        bus->stop();                       \
        if (rspLen < 4) return 1;          \
        if (data[1] != prefix)             \
            return 2;                      \
    }

namespace PS1::IO
{
    enum SIO0_Address : uint8_t
    {
        ADDR_CONTROLLER   = 0x01,
        ADDR_PS2_IR       = 0x21,
        ADDR_PS2_MULTITAP = 0x61,
        ADDR_MEMORY_CARD  = 0x81
    };

    enum SIO0_ResponsePrefix : uint8_t
    {
        PREFIX_CONTROLLER  = 0x5a,
        PREFIX_MEMORY_CARD = 0x5d
    };

    enum Command : uint8_t
    {
        // Basic controller commands
        CMD_POLL   = 'B',
        CMD_CONFIG = 'C',

        // Configuration mode commands
        CMD_INIT_PRESSURE = '@', // DualShock 2 only
        CMD_RESP_INFO     = 'A', // DualShock 2 only
        CMD_SET_ANALOG    = 'D',
        CMD_GET_ANALOG    = 'E',
        CMD_MOTOR_INFO    = 'F',
        CMD_MOTOR_LIST    = 'G',
        CMD_MOTOR_STATE   = 'H',
        CMD_GET_MODES     = 'L',
        CMD_REQ_CONFIG    = 'M',
        CMD_RESP_CONFIG   = 'O', // DualShock 2 only

        // Memory card commands
        CMD_READ_SECTOR   = 'R',
        CMD_IDENTIFY_CARD = 'S', // OEM cards only
        CMD_WRITE_SECTOR  = 'W',

        CMD_AFFIX_MULTITAP_ALL = 0x01, // Tells the multitap to read and respond all four pads
    };

    enum SIOState : uint8_t
    {
        SIO0_OKAY,
        SIO0_IN_USE,
        SIO0_NO_RESPONSE,
    };

    enum MultitapState : uint8_t
    {
        MT_NOT_PRESENT,
        MT_TEST_PRESENCE,
        MT_IS_PRESENT
    };

    enum class SIO0_Port : uint8_t
    {
        PORT1,
        PORT2
    };

    inline constexpr const int sioPortNumber(const SIO0_Port port)
    {
        return (port == SIO0_Port::PORT1 ? 1 : 2);
    }

    enum class Multitap_Port : uint8_t
    {
        PORTA,
        PORTB,
        PORTC,
        PORTD
    };

    inline constexpr const char multitapPortLetter(const Multitap_Port port)
    {
        switch (port)
        {
        case Multitap_Port::PORTA: return 'A';
        case Multitap_Port::PORTB: return 'B';
        case Multitap_Port::PORTC: return 'C';
        case Multitap_Port::PORTD: return 'D';
        }
        return '?';
    }

    inline constexpr Multitap_Port &operator++(Multitap_Port &a)
    {
        auto i = static_cast<uint16_t>(a) + 1;
        if (a == Multitap_Port::PORTD)
            a = Multitap_Port::PORTA;
        else
            a = static_cast<Multitap_Port>(i);
        return a;
    }

    inline constexpr Multitap_Port operator++(Multitap_Port &a, int)
    {
        auto i = static_cast<uint16_t>(a) + 1;
        if (a == Multitap_Port::PORTD)
            a = Multitap_Port::PORTA;
        else
            a = static_cast<Multitap_Port>(i);
        return a;
    }

    inline constexpr Multitap_Port &operator--(Multitap_Port &a)
    {
        auto i = static_cast<uint16_t>(a) + 1;
        if (a == Multitap_Port::PORTA)
            a = Multitap_Port::PORTD;
        else
            a = static_cast<Multitap_Port>(i);
        return a;
    }

    inline constexpr Multitap_Port operator--(Multitap_Port &a, int)
    {
        auto i = static_cast<uint16_t>(a) + 1;
        if (a == Multitap_Port::PORTA)
            a = Multitap_Port::PORTD;
        else
            a = static_cast<Multitap_Port>(i);
        return a;
    }

    struct SIO0_Packet
    {

        SIO0_Port port;
        int length;
    };

    class SIO0_Bus : public ::System::IDriver
    {
        friend class PS1_BaseSystem;

    private:
        bool _initialised = false;
        int _initResult   = 0;
        bool _inUse       = false;

        volatile bool awaiting_ack = false;
        volatile bool ack_received = false;

        MultitapState _multitap[2] = {MT_TEST_PRESENCE, MT_TEST_PRESENCE};

        void m_sioISR();

    public:
        inline SIO0_Bus(::System::ISystem &sys) : ::System::IDriver(sys) {}
        int init() override;
        int start(uint8_t address, SIO0_Port port);
        void stop();
        uint8_t exchangeByte(uint8_t value);
        size_t exchangeBytes(
            const uint8_t *request,
            uint8_t *response,
            size_t reqLength,
            size_t maxRespLength,
            bool hasLastACK = false);

        bool update() override;

        ::IO::Player psxPlayerSelect(SIO0_Port port, Multitap_Port subport);

        // Multitap code - Set and used by the controller drivers, also used by memory card drivers
        inline bool multitapPresent(SIO0_Port port)
        {
            uint8_t _port = (port == SIO0_Port::PORT1 ? 0 : 1);
            return _multitap[_port] == MT_IS_PRESENT;
        }

        inline MultitapState getMultitapState(SIO0_Port port)
        {
            uint8_t _port = (port == SIO0_Port::PORT1 ? 0 : 1);
            return _multitap[_port];
        }

        inline void setMultitapState(SIO0_Port port, MultitapState state)
        {
            uint8_t _port    = (port == SIO0_Port::PORT1 ? 0 : 1);
            _multitap[_port] = state;
        }

        void mouseFix();
    };
} // namespace PS1::IO