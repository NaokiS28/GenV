/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * timer.c - Created on 15-08-2025
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

#ifndef PSX_SYS_TIMER_C
#define PSX_SYS_TIMER_C

#include "timer.h"
#include "../registers.hpp"

uint16_t psx_timer_set_params(uint8_t timer, uint16_t params)
{
    if (timer > PSX_TIMER_2) return 0;
    TIMER_CTRL(timer) = (params & 0x3FF);
    return (params & 0x3FF);
}

uint16_t psx_timer_get_value(uint8_t timer)
{
    if (timer > PSX_TIMER_2) return 0;
    return TIMER_VALUE(timer);
}

uint16_t psx_timer_get_params(uint8_t timer)
{
    if (timer > PSX_TIMER_2) return 0;
    return TIMER_CTRL(timer);
}

bool psx_timer_reached_target(uint8_t timer)
{
    if (timer > PSX_TIMER_2) return false;
    return (TIMER_CTRL(timer) & (1 << 11));
}

bool psx_timer_overflowed(uint8_t timer)
{
    if (timer > PSX_TIMER_2) return false;
    return (TIMER_CTRL(timer) & (1 << 12));
}

void psx_timer_enable_irq(uint8_t timer)
{
    if (timer > PSX_TIMER_2) return;
    IRQ_MASK |= 1 << (IRQ_TIMER0 + timer);
}

void psx_timer_ack_irq(uint8_t timer)
{
    if (timer > PSX_TIMER_2) return;
    TIMER_CTRL(timer) |= PSX_TMR_ACK_IRQ;
}

void psx_timer_reset(uint8_t timer)
{
    if (timer > PSX_TIMER_2) return;
    TIMER_VALUE(timer) = 0;
}

uint16_t psx_timer_set_value(uint8_t timer, uint16_t value)
{
    if (timer > PSX_TIMER_2) return 0;
    TIMER_VALUE(timer) = value;
    return TIMER_VALUE(timer);
}

uint16_t psx_timer_set_target(uint8_t timer, uint16_t target)
{
    if (timer > PSX_TIMER_2) return 0;
    TIMER_TARGET(timer) = target;
    return TIMER_TARGET(timer);
}

#endif