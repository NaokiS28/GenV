/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * jamma.h - Created on 01-08-2025
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
#include "sys573.hpp"

#include "common/services/input.hpp"

namespace System
{
    enum JAMMAInput : uint32_t
    {
        // SYS573_JAMMA_MAIN
        JAMMA_P2_LEFT = 1 << 0,
        JAMMA_P2_RIGHT = 1 << 1,
        JAMMA_P2_UP = 1 << 2,
        JAMMA_P2_DOWN = 1 << 3,
        JAMMA_P2_BUTTON1 = 1 << 4,
        JAMMA_P2_BUTTON2 = 1 << 5,
        JAMMA_P2_BUTTON3 = 1 << 6,
        JAMMA_P2_START = 1 << 7,
        JAMMA_P1_LEFT = 1 << 8,
        JAMMA_P1_RIGHT = 1 << 9,
        JAMMA_P1_UP = 1 << 10,
        JAMMA_P1_DOWN = 1 << 11,
        JAMMA_P1_BUTTON1 = 1 << 12,
        JAMMA_P1_BUTTON2 = 1 << 13,
        JAMMA_P1_BUTTON3 = 1 << 14,
        JAMMA_P1_START = 1 << 15,

        // SYS573_JAMMA_EXT1
        JAMMA_P1_BUTTON4 = 1 << 16,
        JAMMA_P1_BUTTON5 = 1 << 17,
        JAMMA_TEST = 1 << 18,
        JAMMA_P1_BUTTON6 = 1 << 19,

        // SYS573_JAMMA_EXT2
        JAMMA_P2_BUTTON4 = 1 << 20,
        JAMMA_P2_BUTTON5 = 1 << 21,
        JAMMA_RAM_LAYOUT = 1 << 22,
        JAMMA_P2_BUTTON6 = 1 << 23,

        // SYS573_MISC_IN
        JAMMA_COIN1 = 1 << 24,
        JAMMA_COIN2 = 1 << 25,
        JAMMA_PCMCIA_CD1 = 1 << 26,
        JAMMA_PCMCIA_CD2 = 1 << 27,
        JAMMA_SERVICE = 1 << 28
    };

    uint32_t Sys573System::getJAMMAInputs(void)
    {
        uint32_t inputs;

        inputs = SYS573_JAMMA_MAIN;
        inputs |= (SYS573_JAMMA_EXT1 & 0x0f00) << 8;
        inputs |= (SYS573_JAMMA_EXT2 & 0x0f00) << 12;
        inputs |= (SYS573_MISC_IN & 0x1f00) << 16;

        return inputs ^ 0x1fffffff;
    }

    class Sys573Jamma : public Input::IInput
    {
    public:
        // The System 573 JAMMA interface is memory mapped.
        // There's really not much to do other that read in data.
        bool init() { return true; }
        bool reset() { return true; }
        void shutdown() {}
    };
}