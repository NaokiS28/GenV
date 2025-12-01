/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * halt_screen.c - Created on 01-12-2025
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

#ifndef GENV_HALTSCREEN_C
#define GENV_HALTSCREEN_C

#include "halt_screen.h"
#include <stdbool.h>

struct
{
    GenV_HaltScreenFuncs ops;
    bool ready;
} haltscr_settings;

void genv_halt_screen_show(const char *string)
{
    // If this function is called, the system state *cannot* be trusted.
    // The video driver must implement a failsafe halt screen that does not rely on
    // GenV functions
    if (haltscr_settings.ready && haltscr_settings.ops.show != NULL)
    {
        (*haltscr_settings.ops.show)(string);
    }
}

int genv_halt_screen_register(GenV_HaltScreenFuncs *ops)
{
    if (ops->show != NULL)
    {
        haltscr_settings.ops.show = ops->show;
        haltscr_settings.ready = true;
        return 0;
    }
    return -1;
}
#endif