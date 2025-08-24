/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * timer.h - Created on 15-08-2025
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
#ifndef PSX_SYS_TIMER_H
#define PSX_SYS_TIMER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "../registers.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    PSX_TIMER_0 = 0,
    PSX_TIMER_1 = 1,
    PSX_TIMER_2 = 2,
} PSXTimers;

typedef enum
{
    PSX_TMR_SYNC_ENABLE = 1 << 0,
    PSX_TMR_RESET_ON_OVERFLOW = 0 << 3,
    PSX_TMR_RESET_ON_TARGET = 1 << 3,
    PSX_TMR_IRQ_ON_TARGET = 1 << 4,
    PSX_TMR_IRQ_ON_OVERFLOW = 1 << 5,
    PSX_TMR_IRQ_ONCE = 0 << 6,
    PSX_TMR_IRQ_REPEAT = 1 << 6,
    PSX_TMR_IRQ_PULSE_BIT_10 = 0 << 7,
    PSX_TMR_IRQ_TOGGLE_BIT_10 = 1 << 7,
    PSX_TMR_ACK_IRQ = 1 << 10
} PSXTimerFlags;

typedef enum
{
    PSX_TMR0_PAUSE_ON_HBLANK = 0 << 1,
    PSX_TMR0_RESET_ON_HBLANK = 1 << 1,
    PSX_TMR0_RESET_ON_HBLANK_PAUSE = 2 << 1,
    PSX_TMR0_WAIT_FOR_HBLANK = 3 << 1
} PSXTimer0SyncMode;

typedef enum
{
    PSX_TMR1_PAUSE_ON_VBLANK = 0 << 1,
    PSX_TMR1_RESET_ON_VBLANK = 1 << 1,
    PSX_TMR1_RESET_ON_VBLANK_PAUSE = 2 << 1,
    PSX_TMR1_WAIT_FOR_VBLANK = 3 << 1
} PSXTimer1SyncMode;

typedef enum
{
    PSX_TMR2_STOP = 0 << 1,
    PSX_TMR2_RUN = 1 << 1,
} PSXTimer2SyncMode;

typedef enum
{
    // 0 and 1 follow the same pattern, using these values so bit 9 is accounted for
    PSX_TMR0_CLK_SRC_SYSTEM = 2 << 8,
    PSX_TMR0_CLK_SRC_DOTCLK = 3 << 8
} PSXTimer0ClkSrc;

typedef enum
{
    // 0 and 1 follow the same pattern, using these values so bit 9 is accounted for
    PSX_TMR1_CLK_SRC_SYSTEM = 2 << 8,
    PSX_TMR1_CLK_SRC_HBLANK = 3 << 8
} PSXTimer1ClkSrc;

typedef enum
{
    // 0 and 1 follow the same pattern, using these values so bit 9 is accounted for
    PSX_TMR2_CLK_SRC_SYSTEM = 2 << 8,
    PSX_TMR2_CLK_SRC_SYS_DIV8 = 3 << 8
} PSXTimer2ClkSrc;

uint16_t psx_timer_set_params(uint8_t timer, uint16_t params);
uint16_t psx_timer_get_value(uint8_t timer);
uint16_t psx_timer_get_params(uint8_t timer);
bool psx_timer_reached_target(uint8_t timer);
bool psx_timer_overflowed(uint8_t timer);
void psx_timer_enable_irq(uint8_t timer);
void psx_timer_ack_irq(uint8_t timer);
void psx_timer_reset(uint8_t timer);
uint16_t psx_timer_set_value(uint8_t timer, uint16_t value);
uint16_t psx_timer_set_target(uint8_t timer, uint16_t target);

#ifdef __cplusplus
}
#endif

#endif