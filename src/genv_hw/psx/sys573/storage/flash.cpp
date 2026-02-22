/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * flash.c - Created on 26-11-2025
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

#include "flash.hpp"
#include "psx/sys573/registers573.hpp"

uint8_t sys573_bank_state = 0;
void sys573_cart_gpio_set_direction(bool state)
{
    if (state)
        sys573_bank_state |= (state << 6);
    else
        sys573_bank_state &= ~(state << 6);
    SYS573_BANK_CTRL = sys573_bank_state;
}

void sys573_flash_set_bank(uint8_t bank)
{
    bank &= 0x3F;
    if (!(bank & 0x30)) bank &= 0x03;
    uint8_t mask      = sys573_bank_state ^ bank;
    sys573_bank_state = (sys573_bank_state & mask) | bank;
    SYS573_BANK_CTRL  = sys573_bank_state;
}