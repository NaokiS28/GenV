/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * halt.h - Created on 04-12-2025
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
#include "src/halt.h"

#ifndef GENV_573_HALT_H
#define GENV_573_HALT_H

#ifdef __cplusplus
extern "C"
{
#endif

    extern HSDriver sys573_watchdog_driver;
    extern HSExtension sys573_extenstion;

#ifdef __cplusplus
}
#endif

#endif