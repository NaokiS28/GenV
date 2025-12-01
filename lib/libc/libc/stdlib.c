/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * stdlib.c - Created on 12-08-2025
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

#ifndef STDLIB_C
#define STDLIB_C
#include "stdlib.h"
#include "stdio.h"

/* Abort functions */
int genv_halt_screen_show(const char *string);

void _assertAbort(const char *file, int line, const char *expr)
{
    char str[256] = {0};
    snprintf(str, 256, "CRITICAL PROCESS ERROR\r\n\r\n%s:%d: assert(%s)\n", file, line, expr);
#ifndef NDEBUG
    puts(str);
#endif
    genv_halt_screen_show(str);
    for (;;)
        __asm__ volatile("");
}

void abort(void)
{
    char str[256] = {0};
    snprintf(str, 256, "CRITICAL PROCESS ERROR\r\n\r\nabort()\n");
#ifndef NDEBUG
    puts(str);
#endif
    genv_halt_screen_show(str);

    for (;;)
        __asm__ volatile("");
}

void __cxa_pure_virtual(void)
{
    char str[256] = {0};
    snprintf(str, 256, "CRITICAL PROCESS ERROR\r\n\r\n__cxa_pure_virtual()");
#ifndef NDEBUG
    puts(str);
#endif
    genv_halt_screen_show(str);

    for (;;)
        __asm__ volatile("");
}

#endif