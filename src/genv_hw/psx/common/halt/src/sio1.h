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

#pragma once

#ifndef GENV_PS1_SIO0_H
#define GENV_PS1_SIO0_H

#include <stdint.h>
#include "halt.h"

#ifdef __cplusplus
extern "C"
{
#endif
    extern HSConsole halt_console_psx_sio1; // Use this if you want crash details printed to the SIO1 port.
    extern HSConsole halt_console_psx_null; // Use this if you do *NOT* want crash details printed to the SIO1 port.

#ifdef __cplusplus
}
#endif

#endif