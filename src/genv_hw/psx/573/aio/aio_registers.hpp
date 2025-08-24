/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * aio_registers.hpp - Created on 17-08-2025
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

#include "../registers573.hpp"

/* System 573 analog I/O board */

#define SYS573A_LIGHTS_A _MMIO16(DEV0_BASE | 0x640080)
#define SYS573A_LIGHTS_B _MMIO16(DEV0_BASE | 0x640088)
#define SYS573A_LIGHTS_C _MMIO16(DEV0_BASE | 0x640090)
#define SYS573A_LIGHTS_D _MMIO16(DEV0_BASE | 0x640098)
