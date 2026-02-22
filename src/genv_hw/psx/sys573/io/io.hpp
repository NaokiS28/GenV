/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * io.h - Created on 26-11-2025
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
#include <stdbool.h>

#include "psx/sys573/registers573.hpp"
#include "psx/sys573/storage/flash.hpp"

#include "jamma/jamma.hpp"      // IWYU pragma: export
#include "jvs/jvs.hpp"          // IWYU pragma: export
#include "jvs/jvs.hpp"          // IWYU pragma: export
#include "expansion/expbus.hpp" // IWYU pragma: export

namespace System573::IO
{
    inline uint8_t dip_read()
    {
        return (SYS573_DIP_CART & 0x0F);
    }

    inline void ide_set_reset(bool state)
    {
        SYS573_IDE_RESET = !state;
    }

    inline void watchdog_kick()
    {
        SYS573_WATCHDOG = 0;
    }

    // ----------
    // System 573 EXT Out
    // ----------
    namespace EXTOUT
    {
        void set_state(uint8_t data);
        uint8_t get_state();
        void write_bit(uint8_t bit, bool state); // Sets/Unsets a single bit of the output data
    } // namespace EXTOUT

    // ----------
    // Cart FIFO
    // ----------
    namespace SecurityCart
    {
        int set_out_data(uint8_t data);
        int get_in_data(uint8_t *data);

        // ----------
        // Cart GPIO
        // ----------
        inline bool gpio_read_state()
        {
            return (static_cast<MiscInput>(SYS573_MISC_IN) & MiscInput::CART_SDA) != MiscInput::NONE;
        }

        // Writes to D0 of the output port. Note this will trigger the DRDY flag
        inline void gpio_write_state(bool state)
        {
            set_out_data(state);
        }

        // Inline to flash.h GPIO direction function as need to track bank select
        inline void set_direction(bool state)
        {
            sys573_flash_set_gpio(state);
        }
    } // namespace SecurityCart

} // namespace System573::IO