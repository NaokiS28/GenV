/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * sio1.h - Created on 01-12-2025
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

// Self-contained terminal interface library for PS1. Reinits the SIO1 port
// Assumes all prior execution context is invalid - no dependencies on the rest of GenV.

#include <stdint.h>

#include "halt.h"
#include "sio1.h"
#include "ps1/registers.h"

#define SIO1_TICKS_PER_REFRESH 1000 // On the PS1 this doesn't matter, but on watchdog'd devices, this might need changing
static int _sio1_tick_count = 0;

static void _halt_psx_sio1_tick()
{
    _sio1_tick_count++;
    if (_sio1_tick_count >= SIO1_TICKS_PER_REFRESH)
    {
        if (halt_console_psx_sio1.driver_update_cb) halt_console_psx_sio1.driver_update_cb();
        _sio1_tick_count = 0;
    }
}

static int halt_psx_sio1_init(int baud)
{
    SIO_CTRL(1) = SIO_CTRL_RESET;

    SIO_MODE(1) = 0 | SIO_MODE_BAUD_DIV1 | SIO_MODE_DATA_8 | SIO_MODE_STOP_1;
    SIO_BAUD(1) = F_CPU / baud;
    SIO_CTRL(1) = 0 | SIO_CTRL_TX_ENABLE | SIO_CTRL_RX_ENABLE | SIO_CTRL_RTS;
    return 0;
}

static void halt_psx_sio1_putchar(const char c)
{
    if (c == '\n') halt_psx_sio1_putchar('\r');
    _halt_psx_sio1_tick();
    while (!(SIO_STAT(1) & SIO_STAT_TX_NOT_FULL))
        _halt_psx_sio1_tick();
    SIO_DATA(1) = c;
}

static void halt_psx_sio1_puts(const char *str)
{
    while (*str)
    {
        halt_psx_sio1_putchar(*str);
        str++;
    }
    halt_psx_sio1_putchar('\n');
}

static int halt_psx_sio1_null_i(const int arg)
{
    (void)arg;
    return 0;
}

static void halt_psx_sio1_null_cs(const char *arg)
{
    (void)arg;
}

static void halt_psx_sio1_null_c(const char arg)
{
    (void)arg;
}

HSConsole halt_console_psx_sio1 = {
    .init_driver = &halt_psx_sio1_init,
    .puts        = &halt_psx_sio1_puts,
    .putchar     = &halt_psx_sio1_putchar};

HSConsole halt_console_psx_null = {
    .init_driver = &halt_psx_sio1_null_i,
    .puts        = &halt_psx_sio1_null_cs,
    .putchar     = &halt_psx_sio1_null_c};
