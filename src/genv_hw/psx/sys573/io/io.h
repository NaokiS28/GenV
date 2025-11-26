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

#include "../registers573.hpp"
#include "../storage/flash.h"

#ifdef __cplusplus
extern "C"
{
#endif
    inline uint8_t sys573_dip_read()
    {
        return (SYS573_DIP_CART & 0x0F);
    }

    inline void sys573_ide_set_reset(bool state)
    {
        SYS573_IDE_RESET = !state;
    }

    inline void sys573_watchdog_kick()
    {
        SYS573_WATCHDOG = 0;
    }

    // ----------
    // System 573 EXT Out
    // ----------
    void sys573_ext_out_set_state(uint8_t data);
    uint8_t sys573_ext_out_get_state();
    void sys573_ext_output_write_bit(uint8_t bit, bool state); // Sets/Unsets a single bit of the output data

    // ----------
    // Cart FIFO
    // ----------
    int sys573_cart_set_out_data(uint8_t data);
    int sys573_cart_get_in_data(uint8_t *data);

    // ----------
    // Cart GPIO
    // ----------
    inline bool sys573_cart_gpio_read_state()
    {
        return (SYS573_MISC_IN & SYS573_MISC_IN_CART_SDA);
    }

    // Writes to D0 of the output port. Note this will trigger the DRDY flag
    inline void sys573_cart_gpio_write_state(bool state)
    {
        sys573_cart_set_out_data(state);
    }

    // Inline to flash.h GPIO direction function as need to track bank select
    inline void sys573_cart_gpio_set_direction(bool state)
    {
        sys573_flash_set_gpio(state);
    }

#ifdef __cplusplus
}
#endif