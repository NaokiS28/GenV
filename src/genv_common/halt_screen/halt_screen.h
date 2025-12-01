/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * halt_screen.h - Created on 01-12-2025
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
#ifndef GENV_HALTSCREEN_H
#define GENV_HALTSCREEN_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        void (*show)(const char *string);
    } GenV_HaltScreenFuncs;

    void genv_halt_screen_show(const char *string);
    int genv_halt_screen_register(GenV_HaltScreenFuncs *ops);

#ifdef __cplusplus
}
#endif

#endif