/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * terminal.c - Created on 11-08-2025
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

#ifndef GENV_TERMINAL_C
#define GENV_TERMINAL_C

#include <stdbool.h>
#include "terminal.h"

struct
{
    GenV_ConsoleOps ops;
    int baud;
    bool ready;
} tty_settings;

int genv_tty_init(int baud)
{
    if (tty_settings.ops.init != NULL)
    {
        return (*tty_settings.ops.init)(baud);
    }
    return -1;
}

int genv_tty_write(char c)
{
    const char *cp = &c;
    if (tty_settings.ops.write != NULL)
    {
        return (*tty_settings.ops.write)(cp, 1);
    }
    return -1;
}

int genv_tty_write_bytes(const char *data, size_t n)
{
    if (tty_settings.ops.write != NULL)
    {
        return (*tty_settings.ops.write)(data, n);
    }
    return -1;
}

int genv_tty_write_str(const char *str)
{
    if (tty_settings.ops.write != NULL)
    {
        size_t length = 0;
        for (; *&str[length]; length++);
        return (*tty_settings.ops.write)(str, length);
    }
    return -1;
}

int genv_tty_read(void)
{
    char c = 0;
    int r = 0;
    if (tty_settings.ops.read != NULL)
    {
        r = (*tty_settings.ops.read)(&c, 1);
        if (r != 0)
            return r;
        else
            return c;
    }
    return -1;
}

int genv_tty_read_bytes(char *data, size_t n)
{
    if (tty_settings.ops.read != NULL)
    {
        return (*tty_settings.ops.read)(data, n);
    }
    return -1;
}

int genv_tty_register(GenV_ConsoleOps *ops)
{
    if(ops->init != NULL && ops->read != NULL && ops->read != NULL){
        tty_settings.ops.init = ops->init;
        tty_settings.ops.read = ops->read;
        tty_settings.ops.write = ops->write;
        tty_settings.ops.flush = ops->flush;
        return 0;
    }
    return -1;
}
#endif