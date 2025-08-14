/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * terminal.h - Created on 11-08-2025
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
 *  GenV Terminal Handler
 *  ======================
 *
 *  Intermediate layer that handles passing data from libc puts, putchar and printf to
 *  the system's UART port. A system class should register a minimum of a init, read and
 *  write function and do so early on.
 */

#pragma once
#ifndef GENV_TERMINAL_H
#define GENV_TERMINAL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        int (*write)(const char *data, size_t n);
        int (*read)(char *out, size_t n);
        int (*init)(int baud);
        void (*flush)(void);
    } GenV_ConsoleOps;

    int genv_tty_init(int baud);
    int genv_tty_write(char c);
    int genv_tty_write_bytes(const char *data, size_t n);
    int genv_tty_write_str(const char *str);
    int genv_tty_read(void);
    int genv_tty_read_bytes(char *data, size_t n);
    int genv_tty_register(GenV_ConsoleOps *ops);

#ifdef __cplusplus
}
#endif

#endif