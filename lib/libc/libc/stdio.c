/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * stdio.c - Created on 11-08-2025
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

#include "stdio.h"
#include <stddef.h>

// Forwards characters to the terminal driver
int genv_tty_init(void);
int genv_tty_write(char c);
int genv_tty_write_bytes(const char *data, size_t n);
int genv_tty_write_str(const char *str);
int genv_tty_read(void);
int genv_tty_read_bytes(const char *data, size_t n);

int _puts(const char *str)
{
    return genv_tty_write_str(str);
}

void _putchar(char c)
{
    genv_tty_write(c);
}

int _getchar()
{
    return genv_tty_read();
}